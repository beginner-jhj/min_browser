#include "gui/main_window.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include "html/html_tokenizer.h"
#include "html/html_parser.h"
#include <QFile>

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),m_header(nullptr), m_renderer(nullptr){
    m_network_manager = new QNetworkAccessManager(this);
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

    //todo: complete rendering logic. tokenizing, parsing, set_document
    m_renderer = new Renderer(rendering_scroll_area);

    std::shared_ptr<Node> tree = create_tree(m_init_html);
    m_renderer->set_document(tree);

    rendering_scroll_area->setWidget(m_renderer);
    rendering_scroll_area->setWidgetResizable(true);

    layout->addWidget(rendering_scroll_area,1);
}

std::shared_ptr<Node> MainWindow::create_tree(const std::string& html){
    auto tokens = tokenize(html);
    auto tree = parse(tokens);
    return tree;
}

void MainWindow::set_connections(){
    connect(m_header, &Header::file_selected, this, &MainWindow::render_file);
    connect(m_header, &Header::reset, this, [this](){
        auto tree = create_tree(m_init_html);
        m_renderer->set_document(tree);
    });
    connect(m_header, &Header::url_selected, this,&MainWindow::fetch_url);
}

void MainWindow::render_file(const QString &file_path){
    if(file_path.isEmpty()) return;

    QFile file(file_path);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return;
    }

    QTextStream ts(&file);
    QString html_content = ts.readAll();

    auto tree = create_tree(html_content.toStdString());
    m_renderer->set_document(tree);
    file.close();
}

void MainWindow::fetch_url(const QString& url){
    QNetworkRequest request(url);

    QNetworkReply *reply = m_network_manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QString html_content = QString::fromUtf8(data);

            auto tree = create_tree(html_content.toStdString());
            m_renderer->set_document(tree);
        } else {
            qDebug() << "Error:" << reply->errorString();
            auto tree = create_tree(m_network_failed_html);
            m_renderer->set_document(tree);
        }
        reply->deleteLater();
    });
}