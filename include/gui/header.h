#include <QWidget>
#include <QPushButton>
#include <QComboBox>

class Header: public QWidget{
    Q_OBJECT;


    private:
        QPushButton *m_file_open_button;
        QComboBox *m_url_dropdown;

        void draw();
    public:
        Header(QWidget *parent=nullptr);
};