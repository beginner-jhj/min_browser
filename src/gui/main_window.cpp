#include "gui/main_window.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include "html/html_tokenizer.h"
#include "html/html_parser.h"
#include <QFile>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_header(nullptr), m_renderer(nullptr)
{
    m_network_manager = new QNetworkAccessManager(this);
    m_image_cache_manager.image_network_manager = new QNetworkAccessManager(this);
    m_reflow_timer = new QTimer(this);
    m_reflow_timer->setSingleShot(true);
    setup_ui();
    set_connections();
}

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

std::shared_ptr<Node> MainWindow::create_tree(const std::string &html)
{
    auto tokens = tokenize(html);
    auto tree = parse(tokens);
    return tree;
}

void MainWindow::set_connections()
{
    connect(m_header, &Header::file_selected, this, &MainWindow::render_file);
    connect(m_header, &Header::reset, this, [this]()
            {
        m_cached_tree = create_tree(m_init_html);
        m_renderer->set_document(m_cached_tree, m_image_cache_manager); });
    connect(m_header, &Header::url_selected, this, &MainWindow::fetch_url);
    connect(m_image_cache_manager.image_network_manager, &QNetworkAccessManager::finished, this, &MainWindow::download_image);
    connect(m_reflow_timer, &QTimer::timeout, this, &MainWindow::reflow);
}

void MainWindow::render_file(const QString &file_path)
{
    if (file_path.isEmpty())
        return;

    QFile file(file_path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QByteArray data = file.readAll();
    QString html_content = QString::fromUtf8(data);

    QFileInfo file_info(file_path);
    m_cached_base_url = QUrl::fromLocalFile(
                            file_info.absolutePath() + "/")
                            .toString();

    m_cached_tree = create_tree(html_content.toStdString());
    m_renderer->set_document(m_cached_tree, m_image_cache_manager, m_cached_base_url);
    file.close();
}

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
            qDebug() << "Error:" << reply->errorString();
            auto tree = create_tree(m_network_failed_html);
            m_renderer->set_document(tree, m_image_cache_manager);
        }
        reply->deleteLater(); });
}

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

void MainWindow::request_reflow()
{
    m_reflow_timer->start(100);
}

void MainWindow::reflow()
{
    m_renderer->set_document(m_cached_tree, m_image_cache_manager, m_cached_base_url);
}