#include "gui/main_window.h"
#include "gui/header.h"
#include <QVBoxLayout>

MainWindow::MainWindow(){
    setup_ui();
}


void MainWindow::setup_ui()
{
    setMinimumSize(600, 400);
    QWidget *centeral_widget = new QWidget(this);
    setCentralWidget(centeral_widget);

    QVBoxLayout *layout = new QVBoxLayout(centeral_widget);

    Header *header = new Header(this);

    layout->addWidget(header);
}