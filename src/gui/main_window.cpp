#include "gui/main_window.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include "html/html_tokenizer.h"
#include "html/html_parser.h"
#include <QFile>
#include <QFileInfo>

/**
 * \brief Constructs the main browser window with header and renderer.
 *
 * Initializes the main application window with a header bar, rendering area,
 * network manager, image cache manager, and reflow timer. Sets up the central
 * widget with a vertical layout.
 *
 * \param parent The parent QWidget for ownership.
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_header(nullptr), m_renderer(nullptr)
{
    m_network_manager = new QNetworkAccessManager(this);
    m_image_cache_manager.image_network_manager = new QNetworkAccessManager(this);
    m_reflow_timer = new QTimer(this);
    m_reflow_timer->setSingleShot(true);
    setup_ui();
    set_connections();
}

/**
 * \brief Sets up the main window UI with header and renderer.
 *
 * Creates the central widget, vertical layout, header bar, and scrollable
 * rendering area. Initializes the renderer with default HTML content.
 */
void MainWindow::setup_ui()
{
    setMinimumSize(1000, 800);
    QWidget *centeral_widget = new QWidget(this);
    setCentralWidget(centeral_widget);

    QVBoxLayout *layout = new QVBoxLayout(centeral_widget);

    m_header = new Header(this);

    layout->addWidget(m_header);

    QScrollArea *rendering_scroll_area = new QScrollArea(this);

    // todo: complete rendering logic. tokenizing, parsing, set_document
    m_renderer = new Renderer(rendering_scroll_area);

    m_cached_tree = create_tree(m_init_html);
    m_renderer->set_document(m_cached_tree, m_image_cache_manager);

    rendering_scroll_area->setWidget(m_renderer);
    rendering_scroll_area->setWidgetResizable(true);

    layout->addWidget(rendering_scroll_area, 1);
}

/**
 * \brief Tokenizes and parses HTML into a DOM tree.
 *
 * Converts an HTML string into tokens, then parses the tokens into a
 * complete DOM tree structure ready for styling and layout.
 *
 * \param html The HTML source code to parse.
 * \return A shared pointer to the root Node of the parsed DOM tree.
 */
std::shared_ptr<NODE> MainWindow::create_tree(const std::string &html)
{
    auto tokens = tokenize(html);
    auto tree = parse(tokens);
    return tree;
}

/**
 * \brief Establishes signal-slot connections for the main window.
 *
 * Connects header signals to main window slots for navigation, file loading,
 * URL fetching, image downloading, and DOM reflowing. Handles browser history
 * navigation (back/forward) and link click events from the renderer.
 */
void MainWindow::set_connections()
{
    connect(m_header, &Header::file_selected, this, &MainWindow::render_file);
    connect(m_header, &Header::reset, this, [this]()
            {
        m_cached_tree = create_tree(m_init_html);
        m_renderer->set_document(m_cached_tree, m_image_cache_manager); });
    connect(m_header, &Header::url_selected, this, &MainWindow::fetch_url);
    connect(m_header, &Header::back_clicked, m_renderer, &Renderer::go_back);
    connect(m_header, &Header::forward_clicked, m_renderer, &Renderer::go_forward);
    connect(m_image_cache_manager.image_network_manager, &QNetworkAccessManager::finished, this, &MainWindow::download_image);
    connect(m_reflow_timer, &QTimer::timeout, this, &MainWindow::reflow);

    connect(m_renderer, &Renderer::link_clicked, this, &MainWindow::navigate);

}

/**
 * \brief Loads and renders an HTML file from the local filesystem.
 *
 * Opens a local HTML file, reads its content, parses it into a DOM tree,
 * and renders it. Extracts the file's directory as the base URL for relative
 * resource resolution.
 *
 * \param file_path The path to the HTML file to load.
 */
void MainWindow::render_file(const QString &file_path)
{
    if (file_path.isEmpty())
        return;

    QUrl url(file_path);
    QString local_path = url.isLocalFile() ? url.toLocalFile() : file_path;

    QFile file(local_path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QByteArray data = file.readAll();
    QString html_content = QString::fromUtf8(data);

    QFileInfo file_info(local_path);
    m_cached_base_url = QUrl::fromLocalFile(
                            file_info.absolutePath() + "/")
                            .toString();

    m_cached_tree = create_tree(html_content.toStdString());
    m_renderer->set_document(m_cached_tree, m_image_cache_manager, m_cached_base_url);
    file.close();
}

/**
 * \brief Fetches and renders HTML content from a network URL.
 *
 * Makes an asynchronous HTTP/HTTPS request to fetch HTML content from a URL,
 * parses it into a DOM tree, and renders it. Extracts the base URL from the
 * request URL for relative resource resolution. Shows error page if the fetch fails.
 *
 * \param url The URL to fetch and render.
 */
void MainWindow::fetch_url(const QString &url)
{
    QNetworkRequest request(url);

    QNetworkReply *reply = m_network_manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply, url]()
            {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QString html_content = QString::fromUtf8(data);

            m_cached_tree = create_tree(html_content.toStdString());

            QUrl paresed_url(url);
            m_cached_base_url = paresed_url.scheme() + "://" + paresed_url.host() + paresed_url.path();

            int last_slash = m_cached_base_url.lastIndexOf('/');
            if(last_slash != -1){
                m_cached_base_url = m_cached_base_url.left(last_slash +1);
            }
            m_renderer->set_document(m_cached_tree, m_image_cache_manager ,m_cached_base_url);
        } else {
            auto tree = create_tree(m_network_failed_html);
            m_renderer->set_document(tree, m_image_cache_manager);
        }
        reply->deleteLater(); });
}

/**
 * \brief Handles completion of image download from the network.
 *
 * Caches downloaded images in the image cache manager and triggers a reflow
 * to update the rendered page with the newly available image.
 *
 * \param reply The QNetworkReply containing the downloaded image data.
 */
void MainWindow::download_image(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QPixmap image;
        image.loadFromData(reply->readAll());

        m_image_cache_manager.image_cacher[m_image_cache_manager.src] = image;
        request_reflow();
    }
    reply->deleteLater();
}

/**
 * \brief Requests a delayed reflow of the rendered document.
 *
 * Starts a timer to defer DOM reflowing (layout recalculation) to batch
 * layout operations and improve performance when multiple updates occur.
 */
void MainWindow::request_reflow()
{
    m_reflow_timer->start(100);
}

/**
 * \brief Recalculates the layout and re-renders the current document.
 *
 * Triggered by the reflow timer, updates the layout tree to account for
 * newly loaded images or other changes to the document.
 */
void MainWindow::reflow()
{
    m_renderer->set_document(m_cached_tree, m_image_cache_manager, m_cached_base_url);
}

/**
 * \brief Navigates to a URL or file path clicked in the renderer.
 *
 * Routes link clicks to appropriate handlers: local file:// URLs are loaded
 * as files, HTTP/HTTPS URLs are fetched from the network.
 *
 * \param link The URL or file path to navigate to.
 */
void MainWindow::navigate(const QString &link)
{
    if (link.startsWith("file://"))
    {
        render_file(link);
    }

    else if (link.startsWith("http://") || link.startsWith("https://"))
    {
        fetch_url(link);
    }
}