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

private:
    int screenRows_;
    int screenCols_;

    int cursorRow_;
    int cursorCol_;
    
    std::vector<std::vector<ScreenCell>> cells_;

    bool cursorVisible_{true};
    void scrollUp();
    std::vector<std::vector<ScreenCell>> scrollback_;
    int scrollOffset_=0;
    static const int MAX_SCROLLBACK=1000;
    
};