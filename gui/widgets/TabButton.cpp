#include "TabButton.h"

TabButton::TabButton(const QString &title,
                     QWidget *parent)
    : QPushButton(title, parent)
{
    setCheckable(false);
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::NoFocus);

    setMinimumHeight(32);
    setMinimumWidth(120);

    setActive(false);
}

void TabButton::setActive(bool active)
{
    active_ = active;

    if (active_)
    {
        setStyleSheet(
            "QPushButton {"
            "background:#2d2d2d;"
            "color:white;"
            "border:none;"
            "border-bottom:2px solid #4a6cf7;"
            "padding:6px 14px;"
            "}"
        );
    }
    else
    {
        setStyleSheet(
            "QPushButton {"
            "background:transparent;"
            "color: grey;"
            "border:none;"
            "padding:6px 14px;"
            "}"
            "QPushButton:hover {"
            "background:#252525;"
            "}"
        );
    }
}