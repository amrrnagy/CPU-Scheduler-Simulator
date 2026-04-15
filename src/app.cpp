#include <QApplication>
#include "gui/MainWindow.h" // Make sure this path points to your actual header

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Create and show our beautiful UI
    MainWindow w;
    w.show();

    return QApplication::exec();
}