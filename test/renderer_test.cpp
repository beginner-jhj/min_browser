#include <QApplication>
#include <QScrollArea>
#include "rendering/browser_widget.h"
#include "html/html_parser.h"
#include "html/html_tokenizer.h"
#include <iostream>
#include "css/layout_tree.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    std::string html = R"(
<html>
<head>
    <style>
        h1 { color: blue; font-size: 32px; }
        p { color: black; font-size: 16px; }
        .red { color: red; }
    </style>
</head>
<body>
    <h1>Hello World!</h1>
    <p>This is a <span class="red">red</span> paragraph.</p>
    <p>Second paragraph.</p>
</body>
</html>
)";

    auto tokens = tokenize(html);
    auto tree = parse(tokens);


    BrowserWidget *widget = new BrowserWidget();
    widget->set_document(tree);

    QScrollArea *scroll = new QScrollArea();
    scroll->setWidget(widget);
    scroll->setWidgetResizable(true);

    scroll->resize(800, 600);
    scroll->show();

    return app.exec();
}