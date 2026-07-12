#include <QApplication>
#include <QDir>
#include <QFile>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QString themePath = QDir(QCoreApplication::applicationDirPath()).filePath("../gui/themes/Dracula.qss");
    QFile file(themePath);
    if(file.open(QIODevice::ReadOnly))
    app.setStyleSheet(file.readAll());

    MainWindow window;
    window.show();

    return app.exec();
}