#pragma once
#include <QWidget>
#include <QPushButton>
#include <QComboBox>

class Header: public QWidget{
    Q_OBJECT


    private:
        QPushButton *m_back_button;
        QPushButton *m_forward_button;
        QPushButton *m_file_open_button;
        QComboBox *m_url_dropdown;
        QPushButton *m_go_button;
        QPushButton *m_reset_button;
        QString m_selected_url;

        void draw();
        void set_connections();

    signals:
        void url_selected(const QString& url);
        void file_selected(const QString& file_path);
        void reset();
        void back_clicked();
        void forward_clicked();
    
    public:
        Header(QWidget *parent=nullptr);
};