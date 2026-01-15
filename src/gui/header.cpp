#include "gui/header.h"
#include <QHBoxLayout>

Header::Header(QWidget *parent) : QWidget(parent), m_file_open_button(nullptr), m_url_dropdown(0)
{
    draw();
}

void Header::draw()
{
    QHBoxLayout *header_box = new QHBoxLayout(this);

    m_file_open_button = new QPushButton("File");
    header_box->addWidget(m_file_open_button, 1);

    m_url_dropdown = new QComboBox(this);
    m_url_dropdown->addItem("http://info.cern.ch/");
    m_url_dropdown->addItem("https://www.spacejam.com/1996/");
    m_url_dropdown->addItem("http://motherfuckingwebsite.com/");
    m_url_dropdown->addItem("http://bettermotherfuckingwebsite.com/");
    header_box->addWidget(m_url_dropdown, 2);
}