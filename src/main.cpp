#include <stdio.h>
#include "gui/main_window.h"
#include <QApplication>

int main(int argc, char *argv[]){
    QApplication app(argc, argv);
    MainWindow browser;
    browser.show();
    return app.exec();
}