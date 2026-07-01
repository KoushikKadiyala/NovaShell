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
private:
    int screenRows_;
    int screenCols_;

    int cursorRow_;
    int cursorCol_;
    
    std::vector<std::vector<ScreenCell>> cells_;

    bool cursorVisible_{true};
};