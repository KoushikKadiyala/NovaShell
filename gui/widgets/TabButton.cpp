#include "TabButton.h"

#include <QLabel>
#include <QPushButton>
#include <QEvent>
#include <QHBoxLayout>
#include <QMouseEvent>

TabButton::TabButton(const QString &title, QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(32);
    setMinimumWidth(120);
    setCursor(Qt::PointingHandCursor);

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 0, 4, 0);
    layout->setSpacing(6);

    label_ = new QLabel(title, this);
    label_->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    closeBtn_ = new QPushButton("×", this);
    closeBtn_->setFixedSize(16, 16);
    closeBtn_->setFocusPolicy(Qt::NoFocus);
    closeBtn_->setCursor(Qt::PointingHandCursor);

    layout->addWidget(label_);
    layout->addStretch();
    layout->addWidget(closeBtn_);

    connect(closeBtn_, &QPushButton::clicked,
            this, &TabButton::closeRequested);

    setActive(false);
}

void TabButton::setActive(bool active)
{
    active_ = active;
    updateStyle();
}

void TabButton::updateStyle()
{
    if (active_)
    {
        setStyleSheet(
            "QWidget {"
            "  background: #2d2d2d;"
            "  border-bottom: 2px solid #4a6cf7;"
            "  border-radius: 4px;"
            "}"
        );
        label_->setStyleSheet("QLabel { color: white; background: transparent; }");
        closeBtn_->setStyleSheet(
            "QPushButton { background: transparent; color: grey; border: none; }"
            "QPushButton:hover { color: white; }"
        );
    }
    else
    {
        setStyleSheet(
            "QWidget {"
            "  background: transparent;"
            "  border-bottom: 2px solid transparent;"
            "  border-radius: 4px;"
            "}"
            "QWidget:hover { background: #252525; }"
        );
        label_->setStyleSheet("QLabel { color: grey; background: transparent; }");
        closeBtn_->setStyleSheet(
            "QPushButton { background: transparent; color: transparent; border: none; }"
            "QPushButton:hover { color: grey; }"
        );
    }
}

void TabButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        emit clicked();
    QWidget::mousePressEvent(event);
}
