#include <QApplication>
#include "gui/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Create and show the UI
    MainWindow w;
    w.show();

    return QApplication::exec();
}