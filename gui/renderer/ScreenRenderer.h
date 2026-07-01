#pragma once

#include <QByteArray>
#include "../screen/ScreenBuffer.h"

class ScreenRenderer
{
public:
    ScreenRenderer();

    void render(const QByteArray &data, ScreenBuffer &buffer);

private:
    ScreenCell currentStyle;
    void flushText(QByteArray &plainText,
                   ScreenBuffer &buffer);

    void applySGR(const QList<int> &codes);
};