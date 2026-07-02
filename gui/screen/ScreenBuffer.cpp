#include "ScreenBuffer.h"

ScreenBuffer::ScreenBuffer(int rows, int cols)
    : screenRows_(rows),
      screenCols_(cols),
      cursorRow_(0),
      cursorCol_(0),
      cells_(rows, std::vector<ScreenCell>(cols))
{
}

void ScreenBuffer::put(const ScreenCell &cell)
{
    if (cursorRow_ >= screenRows_)
        return;

    cells_[cursorRow_][cursorCol_] = cell;

    cursorCol_++;

    if (cursorCol_ >= screenCols_)
    {
        cursorCol_ = 0;
        lineFeed();
    }
}

void ScreenBuffer::carriageReturn()
{
    cursorCol_ = 0;
}

void ScreenBuffer::lineFeed()
{
    cursorRow_++;

    if (cursorRow_ >= screenRows_)
    {
        scrollUp();
    }
}

void ScreenBuffer::moveLeft(int count)
{
    cursorCol_ -= count;

    if (cursorCol_ < 0)
        cursorCol_ = 0;
}

void ScreenBuffer::moveRight(int count)
{
    cursorCol_ += count;

    if (cursorCol_ >= screenCols_)
        cursorCol_ = screenCols_ - 1;
}

void ScreenBuffer::eraseToEndOfLine()
{
    for (int c = cursorCol_; c < screenCols_; ++c)
        cells_[cursorRow_][c] = ScreenCell{};
}


void ScreenBuffer::scrollUp()
{   
    scrollback_.push_back(cells_[0]);
    if((int)scrollback_.size()>MAX_SCROLLBACK)
    {
        scrollback_.erase(scrollback_.begin());
    }
    // Remove the first row
    cells_.erase(cells_.begin());

    // Add a new blank row at the bottom
    cells_.push_back(std::vector<ScreenCell>(screenCols_));

    cursorRow_ = screenRows_ - 1;
    scrollOffset_ = std::min(
    scrollOffset_,
    (int)scrollback_.size());
}
void ScreenBuffer::resize(int rows, int cols)
{
    std::vector<std::vector<ScreenCell>> newCells(
        rows,
        std::vector<ScreenCell>(cols));

    int copyRows = std::min(rows, screenRows_);
    int copyCols = std::min(cols, screenCols_);

    for (int r = 0; r < copyRows; ++r)
    {
        for (int c = 0; c < copyCols; ++c)
        {
            newCells[r][c] = cells_[r][c];
        }
    }

    cells_ = std::move(newCells);

    screenRows_ = rows;
    screenCols_ = cols;

    cursorRow_ = std::clamp(cursorRow_, 0, screenRows_ - 1);
    cursorCol_ = std::clamp(cursorCol_, 0, screenCols_ - 1);
}
int ScreenBuffer::row() const
{
    return cursorRow_;
}

int ScreenBuffer::column() const
{
    return cursorCol_;
}

const std::vector<std::vector<ScreenCell>>& ScreenBuffer::cells() const
{
    return cells_;
}
bool ScreenBuffer::cursorVisible() const
{
    return cursorVisible_;
}

void ScreenBuffer::setCursorVisible(bool visible)
{
    cursorVisible_ = visible;
}
int ScreenBuffer::scrolloffset() const
{
    return scrollOffset_;
}
void ScreenBuffer::setScrollOffset(int offset)
{
    int maxOffset = (int)scrollback_.size();
    scrollOffset_ = std::clamp(offset, 0, maxOffset);
}
int ScreenBuffer::totalRows() const
{
    return (int)scrollback_.size() + screenRows_;
}
int ScreenBuffer::screenRows() const
{
    return screenRows_;
}
const std::vector<std::vector<ScreenCell>> &ScreenBuffer::scrollBack() const
{
    return scrollback_;
}