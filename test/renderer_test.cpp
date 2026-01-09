#include <QApplication>
#include <QScrollArea>
#include "rendering/browser_widget.h"
#include "html/html_parser.h"
#include "html/html_tokenizer.h"
#include <iostream>
#include "css/layout_tree.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // renderer_test.cpp
   std::string html = R"(
<html>
<head>
    <style>
        .underline { text-decoration: underline; }
        .strike { text-decoration: line-through; }
        .overline { text-decoration: overline; }
    </style>
</head>
<body>
    <p class="underline">This text has underline</p>
    <p class="strike">This text is striked through</p>
    <p class="overline">This text has overline</p>
    <p>Normal text with <span class="underline">underlined part</span> in the middle</p>
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