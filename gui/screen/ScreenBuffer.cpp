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
        cursorRow_++;

        if (cursorRow_ >= screenRows_)
            cursorRow_ = screenRows_ - 1;
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
        cursorRow_ = screenRows_ - 1;
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
