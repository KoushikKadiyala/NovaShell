#include "ScreenBuffer.h"

ScreenBuffer::ScreenBuffer(int rows, int cols)
    : screenRows_(rows),
      screenCols_(cols),
      primaryCells_(rows,   std::vector<ScreenCell>(cols)),
      alternateCells_(rows, std::vector<ScreenCell>(cols))
{
}

// -----------------------------------------------------------------------
// Active buffer routing
// -----------------------------------------------------------------------

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

int ScreenBuffer::scrollBottom() const
{
    return scrollBottomRow_ < 0 ? screenRows_ - 1 : scrollBottomRow_;
}

// -----------------------------------------------------------------------
// Text output
// -----------------------------------------------------------------------

void ScreenBuffer::put(const ScreenCell &cell)
{
    auto &cells = activeCells();
    int  &row   = activeCursorRow();
    int  &col   = activeCursorCol();

    if (row >= screenRows_) return;

    cells[row][col] = cell;
    ++col;

    if (col >= screenCols_)
    {
        col = 0;
        lineFeed();
    }
}

// -----------------------------------------------------------------------
// Cursor movement
// -----------------------------------------------------------------------

void ScreenBuffer::carriageReturn()
{
    activeCursorCol() = 0;
}

void ScreenBuffer::lineFeed()
{
    int &row = activeCursorRow();
    int  bot = scrollBottom();

    if (row >= bot)
        scrollUp();
    else
        ++row;
}

void ScreenBuffer::reverseLineFeed()
{
    int &row = activeCursorRow();
    int  top = scrollTopRow_;

    if (row > top)
    {
        --row;
        return;
    }

    // at top of scroll region — insert blank line at top
    auto &cells = activeCells();
    int  bot    = scrollBottom();

    for (int r = bot; r > top; --r)
        cells[r] = cells[r - 1];
    cells[top] = std::vector<ScreenCell>(screenCols_);
}

void ScreenBuffer::moveLeft(int count)
{
    activeCursorCol() = std::max(0, activeCursorCol() - count);
}

void ScreenBuffer::moveRight(int count)
{
    activeCursorCol() = std::min(screenCols_ - 1,
                                  activeCursorCol() + count);
}

void ScreenBuffer::moveCursorUp(int count)
{
    activeCursorRow() = std::max(scrollTopRow_,
                                  activeCursorRow() - count);
}

void ScreenBuffer::moveCursorDown(int count)
{
    activeCursorRow() = std::min(scrollBottom(),
                                  activeCursorRow() + count);
}

// -----------------------------------------------------------------------
// Cursor positioning
// -----------------------------------------------------------------------

void ScreenBuffer::setCursorPosition(int row, int col)
{
    activeCursorRow() = std::clamp(row, 0, screenRows_ - 1);
    activeCursorCol() = std::clamp(col, 0, screenCols_ - 1);
}

void ScreenBuffer::setCursorRow(int row)
{
    activeCursorRow() = std::clamp(row, 0, screenRows_ - 1);
}

void ScreenBuffer::setCursorColumn(int col)
{
    activeCursorCol() = std::clamp(col, 0, screenCols_ - 1);
}

// -----------------------------------------------------------------------
// Cursor save / restore
// -----------------------------------------------------------------------

void ScreenBuffer::saveCursor()
{
    savedRow_ = activeCursorRow();
    savedCol_ = activeCursorCol();
}

void ScreenBuffer::restoreCursor()
{
    activeCursorRow() = std::clamp(savedRow_, 0, screenRows_ - 1);
    activeCursorCol() = std::clamp(savedCol_, 0, screenCols_ - 1);
}

void ScreenBuffer::saveCursorAndEnterAlternate()
{
    saveCursor();
    enterAlternateScreen();
}

void ScreenBuffer::restoreCursorAndLeaveAlternate()
{
    leaveAlternateScreen();
    restoreCursor();
}

// -----------------------------------------------------------------------
// Erase
// -----------------------------------------------------------------------

void ScreenBuffer::eraseToEndOfLine()
{
    auto &cells = activeCells();
    int   row   = activeCursorRow();
    int   col   = activeCursorCol();

    for (int c = col; c < screenCols_; ++c)
        cells[row][c] = ScreenCell{};
}

void ScreenBuffer::eraseToStartOfLine()
{
    auto &cells = activeCells();
    int   row   = activeCursorRow();
    int   col   = activeCursorCol();

    for (int c = 0; c <= col; ++c)
        cells[row][c] = ScreenCell{};
}

void ScreenBuffer::eraseEntireLine()
{
    auto &cells = activeCells();
    int   row   = activeCursorRow();

    std::fill(cells[row].begin(), cells[row].end(), ScreenCell{});
}

void ScreenBuffer::eraseChars(int count)
{
    auto &cells = activeCells();
    int   row   = activeCursorRow();
    int   col   = activeCursorCol();

    for (int c = col; c < std::min(col + count, screenCols_); ++c)
        cells[row][c] = ScreenCell{};
}

void ScreenBuffer::eraseDisplay(int mode)
{
    auto &cells = activeCells();

    switch (mode)
    {
    case 0: // cursor to end of screen
    {
        int row = activeCursorRow();
        int col = activeCursorCol();

        for (int c = col; c < screenCols_; ++c)
            cells[row][c] = ScreenCell{};

        for (int r = row + 1; r < screenRows_; ++r)
            std::fill(cells[r].begin(), cells[r].end(), ScreenCell{});
        break;
    }
    case 1: // beginning to cursor
    {
        int row = activeCursorRow();
        int col = activeCursorCol();

        for (int r = 0; r < row; ++r)
            std::fill(cells[r].begin(), cells[r].end(), ScreenCell{});

        for (int c = 0; c <= col; ++c)
            cells[row][c] = ScreenCell{};
        break;
    }
    case 2: // entire screen
    case 3: // entire screen + scrollback (xterm extension)
        for (auto &row : cells)
            std::fill(row.begin(), row.end(), ScreenCell{});

        if (mode == 3)
            scrollback_.clear();
        break;
    }
}

void ScreenBuffer::clearScreen()
{
    eraseDisplay(2);
    activeCursorRow() = 0;
    activeCursorCol() = 0;
}

// -----------------------------------------------------------------------
// Insert / delete
// -----------------------------------------------------------------------

void ScreenBuffer::insertChars(int count)
{
    auto &cells = activeCells();
    int   row   = activeCursorRow();
    int   col   = activeCursorCol();

    for (int c = screenCols_ - 1; c >= col + count; --c)
        cells[row][c] = cells[row][c - count];

    for (int c = col; c < std::min(col + count, screenCols_); ++c)
        cells[row][c] = ScreenCell{};
}

void ScreenBuffer::deleteChars(int count)
{
    auto &cells = activeCells();
    int   row   = activeCursorRow();
    int   col   = activeCursorCol();
    int   end   = screenCols_ - count;

    for (int c = col; c < end; ++c)
        cells[row][c] = cells[row][c + count];

    for (int c = end; c < screenCols_; ++c)
        cells[row][c] = ScreenCell{};
}

void ScreenBuffer::insertLines(int count)
{
    auto &cells = activeCells();
    int   row   = activeCursorRow();
    int   bot   = scrollBottom();

    for (int r = bot; r >= row + count; --r)
        cells[r] = cells[r - count];

    for (int r = row; r < std::min(row + count, bot + 1); ++r)
        cells[r] = std::vector<ScreenCell>(screenCols_);

    activeCursorCol() = 0;
}

void ScreenBuffer::deleteLines(int count)
{
    auto &cells = activeCells();
    int   row   = activeCursorRow();
    int   bot   = scrollBottom();

    for (int r = row; r <= bot - count; ++r)
        cells[r] = cells[r + count];

    for (int r = bot - count + 1; r <= bot; ++r)
        cells[r] = std::vector<ScreenCell>(screenCols_);

    activeCursorCol() = 0;
}

// -----------------------------------------------------------------------
// Scroll
// -----------------------------------------------------------------------

void ScreenBuffer::scrollUp()
{
    auto &cells = activeCells();
    int   top   = scrollTopRow_;
    int   bot   = scrollBottom();

    if (!useAlternate_ && top == 0)
    {
        // save row into scrollback only when scrolling from top
        if (scrollOffset_ > 0)
            scrollOffset_ = std::min(scrollOffset_ + 1,
                                     (int)scrollback_.size());

        scrollback_.push_back(cells[top]);

        if ((int)scrollback_.size() > MAX_SCROLLBACK)
            scrollback_.erase(scrollback_.begin());
    }

    for (int r = top; r < bot; ++r)
        cells[r] = cells[r + 1];

    cells[bot] = std::vector<ScreenCell>(screenCols_);

    if (activeCursorRow() > bot)
        activeCursorRow() = bot;
}

void ScreenBuffer::scrollUpN(int count)
{
    for (int n = 0; n < count; ++n) scrollUp();
}

void ScreenBuffer::scrollDownN(int count)
{
    auto &cells = activeCells();
    int   top   = scrollTopRow_;
    int   bot   = scrollBottom();

    for (int n = 0; n < count; ++n)
    {
        for (int r = bot; r > top; --r)
            cells[r] = cells[r - 1];
        cells[top] = std::vector<ScreenCell>(screenCols_);
    }
}

void ScreenBuffer::setScrollRegion(int top, int bottom)
{
    scrollTopRow_    = std::clamp(top,    0, screenRows_ - 1);
    scrollBottomRow_ = std::clamp(bottom, 0, screenRows_ - 1);

    // per VT100 spec — cursor moves to home on DECSTBM
    activeCursorRow() = 0;
    activeCursorCol() = 0;
}

// -----------------------------------------------------------------------
// Alternate screen
// -----------------------------------------------------------------------

void ScreenBuffer::enterAlternateScreen()
{
    if (useAlternate_) return;
    useAlternate_ = true;

    for (auto &row : alternateCells_)
        std::fill(row.begin(), row.end(), ScreenCell{});

    alternateRow_ = 0;
    alternateCol_ = 0;
}

void ScreenBuffer::leaveAlternateScreen()
{
    if (!useAlternate_) return;
    useAlternate_ = false;
}

bool ScreenBuffer::isAlternateScreen() const
{
    return useAlternate_;
}

// -----------------------------------------------------------------------
// Terminal modes
// -----------------------------------------------------------------------

void ScreenBuffer::setApplicationCursorKeys(bool on) { appCursorKeys_ = on; }
void ScreenBuffer::setAutoWrap(bool on)               { autoWrap_ = on; }
void ScreenBuffer::setCursorBlinking(bool on)         { cursorBlinking_ = on; }

// -----------------------------------------------------------------------
// Reset
// -----------------------------------------------------------------------

void ScreenBuffer::reset()
{
    for (auto &row : primaryCells_)
        std::fill(row.begin(), row.end(), ScreenCell{});
    for (auto &row : alternateCells_)
        std::fill(row.begin(), row.end(), ScreenCell{});

    primaryRow_   = primaryCol_   = 0;
    alternateRow_ = alternateCol_ = 0;
    useAlternate_ = false;
    scrollOffset_ = 0;
    scrollTopRow_    = 0;
    scrollBottomRow_ = -1;
    cursorVisible_   = true;
    cursorBlinking_  = true;
    savedRow_ = savedCol_ = 0;
    scrollback_.clear();
}

// -----------------------------------------------------------------------
// Resize
// -----------------------------------------------------------------------

void ScreenBuffer::resize(int rows, int cols)
{
    auto resizeBuffer = [rows, cols](std::vector<std::vector<ScreenCell>> &buf)
    {
        for (auto &row : buf) row.resize(cols);
        buf.resize(rows, std::vector<ScreenCell>(cols));
    };

    resizeBuffer(primaryCells_);
    resizeBuffer(alternateCells_);

    screenRows_ = rows;
    screenCols_ = cols;

    primaryRow_   = std::clamp(primaryRow_,   0, rows - 1);
    primaryCol_   = std::clamp(primaryCol_,   0, cols - 1);
    alternateRow_ = std::clamp(alternateRow_, 0, rows - 1);
    alternateCol_ = std::clamp(alternateCol_, 0, cols - 1);

    scrollBottomRow_ = -1; // reset to full screen
    scrollOffset_    = std::clamp(scrollOffset_, 0, (int)scrollback_.size());
}

// -----------------------------------------------------------------------
// Accessors
// -----------------------------------------------------------------------

int  ScreenBuffer::row()    const { return activeCursorRow(); }
int  ScreenBuffer::column() const { return activeCursorCol(); }

bool ScreenBuffer::cursorVisible()          const { return cursorVisible_; }
void ScreenBuffer::setCursorVisible(bool v)       { cursorVisible_ = v;    }

int  ScreenBuffer::screenRows() const { return screenRows_; }
int  ScreenBuffer::screenCols() const { return screenCols_; }

const std::vector<std::vector<ScreenCell>> &ScreenBuffer::cells() const
{
    return activeCells();
}

const std::vector<std::vector<ScreenCell>> &ScreenBuffer::scrollBack() const
{
    return scrollback_;
}

int ScreenBuffer::scrolloffset() const { return scrollOffset_; }

void ScreenBuffer::setScrollOffset(int offset)
{
    if (useAlternate_) return;
    scrollOffset_ = std::clamp(offset, 0, (int)scrollback_.size());
}

int ScreenBuffer::totalRows() const
{
    return (int)scrollback_.size() + screenRows_;
}