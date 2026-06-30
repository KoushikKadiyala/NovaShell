#include "NavigationButtons.h"

#include <QPainter>

NavigationButtons::NavigationButtons(const QColor &color,
                                       QWidget *parent)
    : QPushButton(parent),
      m_color(color)
{
    setFixedSize(14,14);

    setCursor(Qt::PointingHandCursor);

    setFlat(true);

    setFocusPolicy(Qt::NoFocus);
}

void NavigationButtons::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(Qt::NoPen);

    painter.setBrush(m_color);

    painter.drawEllipse(rect().adjusted(1,1,-1,-1));
}