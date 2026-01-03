#include <iostream>
#include <QApplication>
#include <QScrollArea>
#include "rendering/browser_widget.h"
#include "html/html_parser.h"
#include "html/html_tokenizer.h"
#include "css/css_parser.h"
#include "css/apply_style.h"
#include <queue>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

   std::string html = R"(
<html>
<head>
    <style>
        h1 {
            color: blue;
            font-size: 48px;
        }
        
        p {
            font-size: 16px;
            color: black;
        }
        
        .red-text {
            color: red;
            font-size: 20px;
        }
        
        .green-bold {
            color: green;
            font-size: 24px;
            font-weight: bold;
        }
        
        #purple-heading {
            color: #ff00ff;
            font-size: 2cm;
        }
        
        .transparent {
            color: rgba(117, 33, 77, 0.58);
            font-size: 15pt;
        }
        
        ul {
            font-size: 18px;
        }
        
        li {
            color: darkblue;
        }
        
        .xx-small { 
            font-size: xx-small; 
            color: darkblue; 
        }
        
        .x-small { 
            font-size: x-small; 
            color: crimson; 
        }
        
        .small { 
            font-size: small; 
        }
        
        .medium { 
            font-size: medium; 
        }
        
        .large { 
            font-size: large; 
        }
        
        .x-large { 
            font-size: x-large; 
        }
        
        .xx-large { 
            font-size: xx-large; 
        }
        
        .big-text { 
            font-size: 10mm; 
        }
        
        .cyan-link { 
            color: cyan; 
            font-size: 0.5in; 
        }
        
        .size-8 { font-size: 8px; }
        .size-12 { font-size: 12px; }
        .size-24 { font-size: 24px; }
        .size-32 { font-size: 32px; }
        .size-64 { font-size: 64px; }
    </style>
</head>
<body>
    <h1>Blue 48px Heading</h1>
    <p class="red-text">Red 20px paragraph</p>
    <p>Normal 16px with <strong class="green-bold">green 24px bold</strong> text.</p>
    <h2 id="purple-heading">Purple 2cm H2</h2>
    <p class="transparent">Transparent text using RGBA, 15pt</p>
    
    <ul>
        <li class="xx-small">xx-small Dark Blue</li>
        <li class="x-small">x-small Crimson</li>
        <li class="small">small Normal</li>
        <li class="medium">medium Normal</li>
        <li class="large">large Normal</li>
        <li class="x-large">x-large Normal</li>
        <li class="xx-large">xx-large Normal</li>
    </ul>
    
    <p class="big-text">Visit <a href="#" class="cyan-link">cyan 0.5 inch link</a> for more.</p>
    
    <p class="size-8">Tiny 8px</p>
    <p class="size-12">Small 12px</p>
    <p>Normal 16px</p>
    <p class="size-24">Big 24px</p>
    <p class="size-32">Huge 32px</p>
    <p class="size-64">Giant 64px</p>
</body>
</html>
)";

    auto tokens = tokenize(html);
    auto tree = parse(tokens);

    std::string css = extract_stylesheets(tree);
    CSSOM cssom = create_cssom(css);

    apply_style(tree, cssom);


    // BrowserWidget *widget = new BrowserWidget();
    // widget->set_document(tree);

    // QScrollArea *scroll = new QScrollArea();
    // scroll->setWidget(widget);
    // scroll->setWidgetResizable(true);

    // widget->setMinimumHeight(2000);

    // scroll->resize(600, 400);
    // scroll->show();

    return app.exec();
}