#pragma once

#include <vector>
#include <QChar>

#include "ScreenCell.h"

class ScreenBuffer
{
public:
    ScreenBuffer(int rows = 24, int cols = 80);

    void put(const ScreenCell &cell);

    void carriageReturn();
    void lineFeed();

    void moveLeft(int count = 1);
    void moveRight(int count = 1);

    void eraseToEndOfLine();

    int row() const;
    int column() const;

    const std::vector<std::vector<ScreenCell>>& cells() const;

    bool cursorVisible() const;
    void setCursorVisible(bool visible);


    void resize(int rows, int cols);
    const std::vector<std::vector<ScreenCell>>& scrollBack() const;
    int scrolloffset() const;
    void setScrollOffset(int offset);
    int totalRows() const;
    
    int screenRows() const;
    int screenCols() const;
    void deleteChars(int count);

    void enterAlternateScreen();
    void leaveAlternateScreen();

    bool isAlternateScreen() const;

    void setCursorPosition(int row,int col);
    void setCursorRow(int row);
    void setCursorColumn(int col);
    void clearScreen();
    void eraseDisplay(int mode);

private:
    int screenRows_;
    int screenCols_;

    bool cursorVisible_{true};
    void scrollUp();
    std::vector<std::vector<ScreenCell>> scrollback_;
    int scrollOffset_=0;
    static const int MAX_SCROLLBACK=1000;
    std::vector<std::vector<ScreenCell>> primaryCells_;
    std::vector<std::vector<ScreenCell>> alternateCells_;

    int primaryRow_ = 0;
    int primaryCol_ = 0;

    int alternateRow_ = 0;
    int alternateCol_ = 0;

    bool useAlternate_ = false;

    std::vector<std::vector<ScreenCell>>& activeCells();
    const std::vector<std::vector<ScreenCell>>& activeCells() const;

    int& activeCursorRow();
    int& activeCursorCol();

    int activeCursorRow() const;
    int activeCursorCol() const;
    
};