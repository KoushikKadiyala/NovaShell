#include "ThemeSelector.h"

#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QFile>
#include <QHBoxLayout>
#include <QCoreApplication>
#include <QDir>

ThemeSelector::ThemeSelector(QWidget *parent)
    : QWidget(parent), button(nullptr), menu(nullptr), currentTheme("Dracula")
{
    initUI();
}

void ThemeSelector::initUI()
{
    button = new QPushButton(QStringLiteral("●"), this);
    button->setFixedSize(24, 24);
    button->setStyleSheet(
        "QPushButton {"
        "  border-radius: 12px;"
        "  background-color: #6272a4;"
        "  color: #f8f8f2;"
        "  border: 1px solid #44475a;"
        "}"
        "QPushButton:hover {"
        "  background-color: #7082b4;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #505b84;"
        "}");

    menu = new QMenu(this);
    menu->setStyleSheet(
        "QMenu { background-color: #282a36; color: #f8f8f2; }"
        "QMenu::item:selected { background-color: #44475a; }");

    QStringList themes = {"Dracula", "Dark", "Light", "MaterialDark",
                         "SolarizedDark", "SolarizedLight", "Monokai",
                         "Nord", "GruvboxDark"};

    for (const QString &theme : themes)
    {
        QAction *action = menu->addAction(theme);
        connect(action, &QAction::triggered, this, [this, theme]() {
            setTheme(theme);
        });
    }

    connect(button, &QPushButton::clicked, this, [this]() {
        menu->exec(button->mapToGlobal(button->rect().bottomLeft()));
    });

    auto layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(button);
    setLayout(layout);
}

void ThemeSelector::setTheme(const QString &themeName)
{
    currentTheme = themeName;
    QString themePath = QDir(QCoreApplication::applicationDirPath()).filePath("../gui/themes/"+themeName+".qss");
    QFile file(themePath);
    if (!file.open(QIODevice::ReadOnly))
    {
       qDebug()<<"unable to load theme:"<<themePath;
       return;
    }
    qApp->setStyleSheet(QString::fromUtf8(file.readAll()));
}
