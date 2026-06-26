#include "MainWindow.h"
#include "../widgets/ThemeSelector.h"
#include "../widgets/ShellView.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QFrame>

MainWindow::MainWindow()
    : themeSelector(nullptr)
{
    setWindowTitle("NovaShell");
    resize(1200, 800);

    QWidget *container = new QWidget(this);
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto *titleBar = new QFrame(container);
    titleBar->setObjectName("titleBar");
    titleBar->setFixedHeight(28);
    titleBar->setStyleSheet(
        "QFrame#titleBar { background-color: #282a36; }");

    auto *titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(8, 0, 8, 0);
    titleLayout->setSpacing(8);

    themeSelector = new ThemeSelector(titleBar);
    titleLayout->addWidget(themeSelector);
    titleLayout->addStretch();

    ShellView *shell = new ShellView(container);
    layout->addWidget(titleBar);
    layout->addWidget(shell);

    container->setLayout(layout);
    setCentralWidget(container);
}
