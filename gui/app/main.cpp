#include <QApplication>
#include <QFile>
#include <QDebug>

#include "MainWindow.h"
#include "../gui/pty/PtySession.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QFile file("../gui/themes/Dracula.qss");
    if(file.open(QIODevice::ReadOnly))
    app.setStyleSheet(file.readAll());

    MainWindow window;
    window.show();

    return app.exec();
}