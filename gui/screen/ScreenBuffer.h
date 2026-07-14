#pragma once

#include <vector>
#include <algorithm>
#include <QChar>

#include "ScreenCell.h"

class ScreenBuffer
{
public:
    ScreenBuffer(int rows = 24, int cols = 80);

    // --- text output ---
    void put(const ScreenCell &cell);

    // --- cursor movement ---
    void carriageReturn();
    void lineFeed();
    void reverseLineFeed();
    void moveLeft(int count = 1);
    void moveRight(int count = 1);
    void moveCursorUp(int count = 1);
    void moveCursorDown(int count = 1);

    // --- cursor positioning ---
    void setCursorPosition(int row, int col);
    void setCursorRow(int row);
    void setCursorColumn(int col);

    // --- cursor save/restore ---
    void saveCursor();
    void restoreCursor();
    void saveCursorAndEnterAlternate();
    void restoreCursorAndLeaveAlternate();

    // --- cursor state ---
    int  row()    const;
    int  column() const;
    bool cursorVisible()          const;
    void setCursorVisible(bool v);
    void setCursorBlinking(bool v);

    // --- erase ---
    void eraseToEndOfLine();
    void eraseToStartOfLine();
    void eraseEntireLine();
    void eraseChars(int count);
    void eraseDisplay(int mode);
    void clearScreen();

    // --- insert / delete ---
    void insertChars(int count);
    void deleteChars(int count);
    void insertLines(int count);
    void deleteLines(int count);

    // --- scroll ---
    void scrollUpN(int count);
    void scrollDownN(int count);
    void setScrollRegion(int top, int bottom);

    // --- screen cells ---
    const std::vector<std::vector<ScreenCell>> &cells() const;

    // --- scrollback ---
    const std::vector<std::vector<ScreenCell>> &scrollBack() const;
    int  scrolloffset() const;
    void setScrollOffset(int offset);
    int  totalRows()    const;

    // --- resize ---
    void resize(int rows, int cols);

    // --- dimensions ---
    int screenRows() const;
    int screenCols() const;

    // --- alternate screen ---
    void enterAlternateScreen();
    void leaveAlternateScreen();
    bool isAlternateScreen() const;

    // --- terminal modes ---
    void setApplicationCursorKeys(bool on);
    void setAutoWrap(bool on);

    // --- full reset ---
    void reset();

private:
    int screenRows_;
    int screenCols_;

    bool cursorVisible_{true};
    bool cursorBlinking_{true};
    bool appCursorKeys_{false};
    bool autoWrap_{true};

    // scroll region
    int scrollTopRow_{0};
    int scrollBottomRow_{-1};   // -1 = screenRows_ - 1

    // saved cursor
    int savedRow_{0};
    int savedCol_{0};

    // primary buffer
    std::vector<std::vector<ScreenCell>> primaryCells_;
    int primaryRow_{0};
    int primaryCol_{0};

    // alternate buffer
    std::vector<std::vector<ScreenCell>> alternateCells_;
    int alternateRow_{0};
    int alternateCol_{0};

    bool useAlternate_{false};

    // scrollback (primary only)
    std::vector<std::vector<ScreenCell>> scrollback_;
    int scrollOffset_{0};
    static constexpr int MAX_SCROLLBACK = 2000;

    // helpers
    std::vector<std::vector<ScreenCell>> &activeCells();
    const std::vector<std::vector<ScreenCell>> &activeCells() const;
    int &activeCursorRow();
    int &activeCursorCol();
    int  activeCursorRow() const;
    int  activeCursorCol() const;
    int  scrollBottom()    const;

    void scrollUp();
};