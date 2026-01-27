#include "gui/header.h"
#include <QHBoxLayout>
#include <QFileDialog>

/**
 * \brief Constructs a Header widget with navigation and URL input controls.
 *
 * Initializes a Header widget containing back/forward buttons, file opener,
 * URL dropdown, and action buttons. Sets up the UI layout and signal connections
 * for user interactions.
 *
 * \param parent The parent QWidget for ownership and memory management.
 */
Header::Header(QWidget *parent) : QWidget(parent), m_back_button(nullptr),m_forward_button(nullptr),m_file_open_button(nullptr), m_url_dropdown(nullptr), m_go_button(nullptr), m_reset_button(nullptr)
{
    draw();
    set_connections();
}

/**
 * \brief Builds and displays the header widget UI layout.
 *
 * Creates the horizontal layout with back/forward navigation buttons, file opener button,
 * URL dropdown with preset URLs, go button, and reset button. Sets appropriate sizes
 * and stretch factors for responsive layout.
 */
void Header::draw()
{
    QHBoxLayout *header_box = new QHBoxLayout(this);

    m_back_button = new QPushButton("<-");
    m_back_button->setFixedWidth(40);
    header_box->addWidget(m_back_button);

    m_forward_button = new QPushButton("->");
    m_forward_button->setFixedWidth(40);
    header_box->addWidget(m_forward_button);

    m_file_open_button = new QPushButton("File");
    header_box->addWidget(m_file_open_button,2);

    m_url_dropdown = new QComboBox(this);
    m_url_dropdown->setPlaceholderText("Select an URL");
    m_url_dropdown->addItem("http://info.cern.ch/");
    m_url_dropdown->addItem("http://motherfuckingwebsite.com/");
    header_box->addWidget(m_url_dropdown,3);

    m_go_button = new QPushButton("Go");
    header_box->addWidget(m_go_button,1);

    m_reset_button = new QPushButton("Reset");
    header_box->addWidget(m_reset_button,1);
}

/**
 * \brief Establishes signal-slot connections for header widget interactions.
 *
 * Connects all header buttons and controls to their respective signals.
 * File button opens a file dialog, URL dropdown stores selection, go button
 * emits URL navigation, and navigation buttons emit history signals.
 */
void Header::set_connections(){
    connect(m_file_open_button, &QPushButton::clicked, this, [this](){
    QString file_path = QFileDialog::getOpenFileName(this,"Select a html file.","","Web Files (*.html *.css);;All Files (*.*)");
    if(!file_path.isEmpty()){
        emit file_selected(file_path);
    }
    });

    connect(m_url_dropdown, &QComboBox::currentTextChanged,this, [this](const QString& url){
        if(!url.isEmpty()){
            this->m_selected_url = url;
        }
    });

    connect(m_go_button, &QPushButton::clicked, this, [this](){
        emit url_selected(m_selected_url);
    });

    connect(m_reset_button, &QPushButton::clicked, this, [this](){
        emit reset();
    });

    connect(m_back_button, &QPushButton::clicked, this, [this]{
        emit back_clicked();
    });

    connect(m_forward_button, &QPushButton::clicked, this, [this]{
        emit forward_clicked();
    });
}