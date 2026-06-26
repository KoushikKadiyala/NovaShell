#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class ThemeSelector;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private:
    ThemeSelector *themeSelector;
};

#endif
