#include "ScreenWidget.h"
#include "ScreenBuffer.h"

#include <QPainter>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QFontMetrics>

ScreenWidget::ScreenWidget(ScreenBuffer *buffer,
                           QWidget *parent)
    : QWidget(parent),
      buffer_(buffer)
{
    setAutoFillBackground(true);
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

     connect(&blinkTimer,
            &QTimer::timeout,
            this,
            [this]()
            {
                buffer_->setCursorVisible(
                    !buffer_->cursorVisible());

                update();
            });
    blinkTimer.start(500);

    QFont font("JetBrains Mono",12);
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);

    setFont(font);
    updateFonts();
}
void ScreenWidget::updateFonts()
{
    QFont font = this->font();
    font.setBold(false);
    normalFont_ = font;

    font.setBold(true);
    boldFont_ = font;
}
void ScreenWidget::paintEvent(QPaintEvent*)
{  
    QPainter painter(this);
    painter.setRenderHint(QPainter::TextAntialiasing);

    QFontMetrics fm(normalFont_);
    int cellWidth = fm.horizontalAdvance('W');
    int cellHeight = fm.height();

    const auto &scrollBack = buffer_->scrollBack();
    const auto &cells = buffer_->cells();
    int offset = buffer_->scrolloffset();

    int totalHistory = (int)scrollBack.size();
    int screenRows_ = buffer_->screenRows();
    int startRow = totalHistory - offset;

for (int viewrow = 0; viewrow < screenRows_; ++viewrow)
{   const int srcRow = startRow + viewrow;
    const std::vector<ScreenCell> *row = nullptr;
    if(srcRow<0)
    {
        continue;
    }
    else if(srcRow< totalHistory)
    {
        row = &scrollBack[srcRow];
    }
    else if(srcRow< totalHistory + (int)cells.size())
    {
       row = &cells[srcRow - totalHistory];
    }
    else
    {
        continue;
    }
    for (int col = 0; col < (int)cells[viewrow].size(); ++col)
    {
        const ScreenCell &cell = (*row)[col];

        if (cell.ch == ' ')
            continue;

        painter.setPen(cell.fg);
        painter.setFont(cell.bold? boldFont_ : normalFont_);

        painter.drawText(
            LEFT_MARGIN + col * cellWidth,
            TOP_MARGIN + (viewrow + 1) * cellHeight - fm.descent(),
            QString(cell.ch));
    }
    
}
if(offset==0 && buffer_->cursorVisible())
{   const int cursorRow = buffer_->row();
    const int cursorCol = buffer_->column();

   QRect cursorRect(
        LEFT_MARGIN + cursorCol * cellWidth,
        TOP_MARGIN + cursorRow * cellHeight,
        cellWidth,
        cellHeight);

    painter.fillRect(cursorRect, Qt::white);
}
}
void ScreenWidget::keyPressEvent(QKeyEvent *event)
{  if(buffer_->scrolloffset()>0){
    buffer_->setScrollOffset(0);
    update();
    }
     buffer_->setCursorVisible(true);
    update();
    switch (event->key())
    {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        emit bytesTyped("\r");
        break;

    case Qt::Key_Backspace:
        emit bytesTyped("\b");
        break;
    case Qt::Key_Up: emit bytesTyped("\033[A"); break;
    case Qt::Key_Down: emit bytesTyped("\033[B"); break;
    case Qt::Key_Right: emit bytesTyped("\033[C"); break;
    case Qt::Key_Left: emit bytesTyped("\033[D"); break;

    default:
        if (!event->text().isEmpty())
            emit bytesTyped(event->text().toUtf8());
        break;
    }

    event->accept();
}
void ScreenWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    QFontMetrics fm(normalFont_);

    int cellWidth  = fm.horizontalAdvance('M');
    int cellHeight = fm.height();

    int cols = std::max(1,
        (width() - 2 * LEFT_MARGIN) / cellWidth);

    int rows = std::max(1,
        (height() - 2 * TOP_MARGIN) / cellHeight);

    buffer_->resize(rows, cols);

    emit terminalResized(rows,cols);
}
void ScreenWidget::wheelEvent(QWheelEvent *event)
{   constexpr int SCROLL_STEP = 3;
    const int delta = event->angleDelta().y() > 0 ?
        SCROLL_STEP : -SCROLL_STEP;
    const int MaxOffset = (int)buffer_->scrollBack().size();

    const int newOffset = std::clamp(buffer_->scrolloffset() + delta,0,MaxOffset);
    buffer_->setScrollOffset(newOffset);
    update();
    event->accept();
}