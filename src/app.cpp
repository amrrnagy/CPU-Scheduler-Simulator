#include <QApplication>
#include "gui/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Create and show our beautiful UI
    MainWindow w;
    w.show();

    return QApplication::exec();
}