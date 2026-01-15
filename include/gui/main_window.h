#include <QMainWindow>

class MainWindow: public QMainWindow{
    Q_OBJECT;

    private:
        void setup_ui();

    public:
       explicit MainWindow();
};