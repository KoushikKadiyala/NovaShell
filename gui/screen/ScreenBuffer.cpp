#include "ScreenBuffer.h"

ScreenBuffer::ScreenBuffer(int rows, int cols)
    : screenRows_(rows),
      screenCols_(cols),
     primaryCells_(rows,std::vector<ScreenCell>(cols)),
     alternateCells_(rows,std::vector<ScreenCell>(cols))
{
}
void ScreenBuffer::put(const ScreenCell &cell)
{   auto &cells = activeCells();
    int &row =activeCursorRow();
    int &col = activeCursorCol();

    if(row >= screenRows_)
        return;
    cells[row][col] = cell;

    col++;

    if(col >= screenCols_)
    {
        col = 0;
        lineFeed();
    }

    // if (cursorRow_ >= screenRows_)
    //     return;

    // cells_[cursorRow_][cursorCol_] = cell;

    // cursorCol_++;

    // if (cursorCol_ >= screenCols_)
    // {
    //     cursorCol_ = 0;
    //     lineFeed();
    // }
}
void ScreenBuffer::carriageReturn()
{
    activeCursorCol() = 0;
}
void ScreenBuffer::lineFeed()
{
    activeCursorRow()++;

    if (activeCursorRow() >= screenRows_)
        scrollUp();
}
void ScreenBuffer::moveLeft(int count)
{
    activeCursorCol() -= count;

    if (activeCursorCol() < 0)
        activeCursorCol() = 0;
}
void ScreenBuffer::moveRight(int count)
{
    activeCursorCol() += count;

    if (activeCursorCol() >= screenCols_)
        activeCursorCol() = screenCols_ - 1;
}
void ScreenBuffer::eraseToEndOfLine()
{
    auto &cells = activeCells();
    int row = activeCursorRow();
    int col = activeCursorCol();

    for (int c = col; c < screenCols_; ++c)
        cells[row][c] = ScreenCell{};
}
void ScreenBuffer::scrollUp()
{
    auto &cells = activeCells();

    if (!useAlternate_)
    {
        // Normal screen -> keep scrollback
        scrollback_.push_back(cells.front());

        if ((int)scrollback_.size() > MAX_SCROLLBACK)
            scrollback_.erase(scrollback_.begin());

        cells.erase(cells.begin());
        cells.push_back(std::vector<ScreenCell>(screenCols_));

        activeCursorRow() = screenRows_ - 1;

        scrollOffset_ = std::min(
            scrollOffset_,
            (int)scrollback_.size());
    }
    else
    {
        // Alternate screen -> NO scrollback
        cells.erase(cells.begin());
        cells.push_back(std::vector<ScreenCell>(screenCols_));

        activeCursorRow() = screenRows_ - 1;
    }
}
void ScreenBuffer::resize(int rows, int cols)
{
    auto resizeBuffer = [rows, cols](std::vector<std::vector<ScreenCell>> &buffer)
    {
        // Resize existing rows
        for (auto &row : buffer)
            row.resize(cols);

        // Resize number of rows
        buffer.resize(rows, std::vector<ScreenCell>(cols));
    };

    resizeBuffer(primaryCells_);
    resizeBuffer(alternateCells_);

    screenRows_ = rows;
    screenCols_ = cols;

    primaryRow_ = std::clamp(primaryRow_, 0, rows - 1);
    primaryCol_ = std::clamp(primaryCol_, 0, cols - 1);

    alternateRow_ = std::clamp(alternateRow_, 0, rows - 1);
    alternateCol_ = std::clamp(alternateCol_, 0, cols - 1);

    scrollOffset_ = std::clamp(scrollOffset_, 0, (int)scrollback_.size());
}
int ScreenBuffer::row() const
{
    return activeCursorRow();
}
int ScreenBuffer::column() const
{
    return activeCursorCol();
}
const std::vector<std::vector<ScreenCell>>& ScreenBuffer::cells() const
{
    return activeCells();
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
int ScreenBuffer::screenCols() const
{
    return screenCols_;
}
const std::vector<std::vector<ScreenCell>> &ScreenBuffer::scrollBack() const
{
    return scrollback_;
}
void ScreenBuffer::deleteChars(int count)
{
    auto &cells = activeCells();

    int row = activeCursorRow();
    int col = activeCursorCol();

    int end = screenCols_ - count;

    for (int c = col; c < end; ++c)
        cells[row][c] = cells[row][c + count];

    for (int c = end; c < screenCols_; ++c)
        cells[row][c] = ScreenCell{};
}
std::vector<std::vector<ScreenCell>> &ScreenBuffer::activeCells()
{
    return useAlternate_ ? alternateCells_ : primaryCells_;
}

const std::vector<std::vector<ScreenCell>> &ScreenBuffer::activeCells() const
{
    return useAlternate_ ? alternateCells_ : primaryCells_;
}

int &ScreenBuffer::activeCursorRow()
{
    return useAlternate_ ? alternateRow_ : primaryRow_;
}

int &ScreenBuffer::activeCursorCol()
{
    return useAlternate_ ? alternateCol_ : primaryCol_;
}

int ScreenBuffer::activeCursorRow() const
{
    return useAlternate_ ? alternateRow_ : primaryRow_;
}

int ScreenBuffer::activeCursorCol() const
{
    return useAlternate_ ? alternateCol_ : primaryCol_;
}

void ScreenBuffer::enterAlternateScreen()
{
    if(useAlternate_)
        return;
    useAlternate_ = true;

    for (auto &row : alternateCells_)
        std::fill(row.begin(), row.end(), ScreenCell{});
    alternateRow_ = 0;
    alternateCol_ = 0;
}

void ScreenBuffer::leaveAlternateScreen()
{
    if(!useAlternate_)
        return;
    useAlternate_ = false;
}

bool ScreenBuffer::isAlternateScreen() const
{
    return useAlternate_;
}
void ScreenBuffer::setCursorPosition(int row,int col){
    activeCursorRow() = std::clamp(row, 0, screenRows_ - 1);
    activeCursorCol() = std::clamp(col, 0,screenCols_ - 1);
}
void ScreenBuffer::clearScreen(){
    auto &cells = activeCells();

    for(auto &row : cells)
    {
        std:fill(row.begin(),row.end(),ScreenCell{});
    }
    activeCursorRow() = 0;
    activeCursorCol()= 0;
}
void ScreenBuffer::eraseDisplay(int mode)
{
    auto &cells = activeCells();

    switch (mode)
    {
    case 0:     // Cursor -> end of screen
    {
        int row = activeCursorRow();
        int col = activeCursorCol();

        for (int c = col; c < screenCols_; ++c)
            cells[row][c] = ScreenCell{};

        for (int r = row + 1; r < screenRows_; ++r)
            std::fill(cells[r].begin(), cells[r].end(), ScreenCell{});

        break;
    }

    case 1:     // Beginning -> cursor
    {
        int row = activeCursorRow();
        int col = activeCursorCol();

        for (int r = 0; r < row; ++r)
            std::fill(cells[r].begin(), cells[r].end(), ScreenCell{});

        for (int c = 0; c <= col; ++c)
            cells[row][c] = ScreenCell{};

        break;
    }

    case 2:     // Entire screen
    {
        for (auto &row : cells)
            std::fill(row.begin(), row.end(), ScreenCell{});

        break;
    }
    }
}
void ScreenBuffer::setCursorRow(int row){
    activeCursorRow() = std::clamp(row, 0,screenRows_ - 1);
}

void ScreenBuffer::setCursorColumn(int col){
    activeCursorCol() = std::clamp(col, 0,screenCols_ - 1);
}