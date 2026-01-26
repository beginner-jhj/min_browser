#pragma once
#include <QMainWindow>
#include "html/node.h"
#include <memory>
#include "gui/header.h"
#include "gui/renderer.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <unordered_map>
#include "gui/image_cache_manager.h"
#include <QTimer>

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    void setup_ui();

    std::string m_init_html = R"(
    <!DOCTYPE html>
    <html>
        <head>
        </head>
        <body>
            <! -- comments -- !>
            <h1>🌐 Welcome to My Custom Browser</h1>
            
            <p>A lightweight browser engine built from scratch with C++ and Qt.</p>
            
            <h2>Current Features</h2>
            <ul>
                <li>HTML tokenization and parsing</li>
                <li>CSS tokenization and parsing</li>
                <li>Basic styling and layout engine</li>
                <li>Rendering pipeline implementation</li>
            </ul>
            
            <h2>Design Philosophy</h2>
            <p>Built with a focus on understanding browser internals, 
               this project demonstrates core web rendering concepts 
               without relying on existing browser engines.</p>
            
            <footer>
                <p><em>Developed by 현진</em></p>
            </footer>
        </body>
    </html>
)";

    std::string m_network_failed_html = R"(
<!DOCTYPE html>
<html>
<head>
    <style>
        body { 
            font-family: sans-serif; 
            display: block; 
            padding: 40px; 
            background-color: #f4f4f9; 
            text-align: center;
        }
        div { 
            display: block;
            background: white; 
            padding: 30px; 
            margin: 0 auto;
            max-width: 500px; 
            border-radius: 12px;
            border: 1px solid #ddd;
        }
        h1 { color: #e74c3c; font-size: 24px; margin-bottom: 10px; }
        p { color: #555; line-height: 1.6; margin-bottom: 20px; }
        .footer { font-size: 12px; color: #aaa; margin-top: 20px; }
    </style>
</head>
<body>
    <div>
        <h1>Failed to connect</h1>
        <p>Server error or Invalid URL<br>Check your network.</p>
    </div>
</body>
</html>
)";

    Header *m_header;
    Renderer *m_renderer;
    QNetworkAccessManager *m_network_manager;
    IMAGE_CACHE_MANAGER m_image_cache_manager;
    QTimer *m_reflow_timer;
    std::shared_ptr<Node> m_cached_tree;
    QString m_cached_base_url;

    void set_connections();
    void request_reflow();

public slots:
    void render_file(const QString &file_path);
    void fetch_url(const QString &url);
    void download_image(QNetworkReply *reply);
    void reflow();
    void navigate(const QString &link);

public:
    explicit MainWindow(QWidget *parent = nullptr);
    std::shared_ptr<Node> create_tree(const std::string &html);
};