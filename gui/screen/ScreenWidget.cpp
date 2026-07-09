#include "ScreenWidget.h"
#include "ScreenBuffer.h"

#include <QPainter>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QFontMetrics>
#include <QClipboard>
#include <QApplication>

ScreenWidget::ScreenWidget(ScreenBuffer *buffer,
                           QWidget *parent)
    : QWidget(parent),
      buffer_(buffer)
{   setObjectName("ScreenWidget");
    setAutoFillBackground(false);
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
    for (int col = 0; col < (int)row->size(); ++col)
    {
        const ScreenCell &cell = (*row)[col];

        if (cell.ch == ' ')
            continue;

        painter.setPen(cell.fg);
        QRect cellRect(
        LEFT_MARGIN + col * cellWidth,
        TOP_MARGIN + viewrow * cellHeight,
        cellWidth,
        cellHeight);

        if (isCellSelected(viewrow, col))
        {
            painter.fillRect(cellRect, QColor("#4a6cf7")); // selection color
        }
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

    if((event->modifiers() & Qt::ControlModifier) && (event->modifiers() & Qt::ShiftModifier))
    {
        if(event->key() == Qt::Key_C)
        {
            copySelection();
            event->accept();
            return;
        }

        if(event->key() == Qt::Key_V)
        {
            QString text = QApplication::clipboard()->text();
            emit bytesTyped(text.toUtf8());
            event->accept();
            return;
        }
    }

    buffer_->setCursorVisible(true);
    update();
    switch (event->key())
    {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        emit bytesTyped("\r");
        break;
    case Qt::Key_Tab:
        emit bytesTyped("\t");
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
void ScreenWidget::copySelection()
{
   QString text;
   QPoint a = selectionStart;
   QPoint b = selectionEnd;

   if(a.y()> b.y() || (a.y()==b.y() && a.x()>b.x()))
       std::swap(a,b);
    const auto &cells = buffer_->cells();
    for(int row = a.y(); row<=b.y(); ++row)
    {
        int startCol = (row==a.y())? a.x() : 0;
        int endCol = (row==b.y())? b.x() : buffer_->screenCols()-1;
        for(int col = startCol; col<=endCol; ++col)
        text+=cells[row][col].ch;
        if(row!=b.y())
            text+='\n';

    }
    QApplication::clipboard()->setText(text);
    hasSelection = false;
    selecting = false;
    selectionStart = {};
    selectionEnd = {};
    update();
}
QPoint ScreenWidget::cellFromPosition(const QPointF &pos) const
{
    QFontMetrics fm(normalFont_);

    int cellWidth  = fm.horizontalAdvance('W');
    int cellHeight = fm.height();

    int col = (pos.x() - LEFT_MARGIN) / cellWidth;
    int row = (pos.y() - TOP_MARGIN) / cellHeight;

    col = std::max(0, col);
    row = std::max(0, row);

    return QPoint(col, row);
}

void ScreenWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    selecting = true;
    hasSelection = false;

    selectionStart = cellFromPosition(event->position());
    selectionEnd   = selectionStart;

    update();
}

void ScreenWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!selecting)
        return;

    selectionEnd = cellFromPosition(event->position());
    if(selectionEnd != selectionStart)
        hasSelection = true;

    update();
}

void ScreenWidget::mouseReleaseEvent(QMouseEvent *event)
{
   Q_UNUSED(event);

    selecting = false;

    update();
}

bool ScreenWidget::isCellSelected(int row, int col) const
{   
    if (!hasSelection)
        return false;
    QPoint a = selectionStart;
    QPoint b = selectionEnd;

    // Normalize selection
    if (a.y() > b.y() || (a.y() == b.y() && a.x() > b.x()))
        std::swap(a, b);

    if (row < a.y() || row > b.y())
        return false;

    if (a.y() == b.y())
        return col >= a.x() && col <= b.x();

    if (row == a.y())
        return col >= a.x();

    if (row == b.y())
        return col <= b.x();
    return true;
}