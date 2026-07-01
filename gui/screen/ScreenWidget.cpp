#include "ScreenWidget.h"
#include "ScreenBuffer.h"

#include <QPainter>
#include <QFontMetrics>
#include <QKeyEvent>

ScreenWidget::ScreenWidget(ScreenBuffer *buffer,
                           QWidget *parent)
    : QWidget(parent),
      buffer_(buffer)
{
    setAutoFillBackground(true);
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    QFont font("JetBrains Mono",12);
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);

    setFont(font);
}
void ScreenWidget::paintEvent(QPaintEvent*)
{  
    QPainter painter(this);

    painter.fillRect(rect(), QColor("#282a36"));

    QFontMetrics fm(font());
    int cellWidth = fm.horizontalAdvance('W');
    int cellHeight = fm.height();

    const auto &cells = buffer_->cells();

for (int row = 0; row < (int)cells.size(); ++row)
{
    for (int col = 0; col < (int)cells[row].size(); ++col)
    {
        const ScreenCell &cell = cells[row][col];

        if (cell.ch == ' ')
            continue;

        painter.setPen(cell.fg);
        QFont normal = font();
        normal.setBold(false);

        QFont bold = font();
        bold.setBold(true);
        painter.setFont(cell.bold ? bold : normal);

        painter.drawText(
            col * cellWidth,
            (row + 1) * cellHeight,
            QString(cell.ch));
    }
    
}
    if (buffer_->cursorVisible())
{
    int row = buffer_->row();
    int col = buffer_->column();

    QRect cursorRect(
        LEFT_MARGIN + col * cellWidth,
        TOP_MARGIN + row * cellHeight,
        2,
        cellHeight);

    painter.fillRect(cursorRect, Qt::white);
}
}

void ScreenWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        emit bytesTyped("\r");
        break;

    case Qt::Key_Backspace:
        emit bytesTyped("\b");
        break;

    default:
        if (!event->text().isEmpty())
            emit bytesTyped(event->text().toUtf8());
        break;
    }

    event->accept();
}