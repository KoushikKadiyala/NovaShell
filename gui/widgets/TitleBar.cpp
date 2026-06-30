#include "TitleBar.h"

#include "ThemeSelector.h"
#include "NavigationButtons.h"

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QLabel>
#include <QWindow>

void TitleBar::mousePressEvent(QMouseEvent *event)
{
    qDebug()<<event->button();
    if (event->button() == Qt::LeftButton)
    {
       
       if(QWindow *win = window()->windowHandle())
       {
        win->startSystemMove();
       }
       event->accept();
       
    }
}

TitleBar::TitleBar(QWidget *parent)
    : QWidget(parent)
{
    setFixedHeight(34);

    setObjectName("TitleBar");
    setAttribute(Qt::WA_StyledBackground,true);

    auto *layout = new QHBoxLayout(this);

    layout->setContentsMargins(10,0,10,0);
    layout->setSpacing(8);

    closeButton =
        new NavigationButtons(QColor("#ff5f56"));

    minimizeButton =
        new NavigationButtons(QColor("#ffbd2e"));

    maximizeButton =
        new NavigationButtons(QColor("#27c93f"));

    auto *title = new QLabel("NovaShell");

    themeSelector = new ThemeSelector;

    connect(closeButton,
            &QPushButton::clicked,
            window(),
            [this](){
                window()->close();
            });

    connect(minimizeButton,
            &QPushButton::clicked,
            this,
            [this](){
                window()->showMinimized();
            });

    connect(maximizeButton,
            &QPushButton::clicked,
            [this]()
            {
                QWidget *w = window();

                if (w->isMaximized())
                    w->showNormal();
                else
                    w->showMaximized();
            });

    layout->addWidget(themeSelector);

    layout->addWidget(title);

    layout->addStretch();

    layout->addWidget(minimizeButton);
    layout->addWidget(maximizeButton);
    layout->addWidget(closeButton);

    
}