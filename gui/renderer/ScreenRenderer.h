#pragma once

#include <QByteArray>
#include <QColor>
#include "../screen/ScreenBuffer.h"

class ScreenRenderer
{
public:
    ScreenRenderer();

    void render(const QByteArray &data, ScreenBuffer &buffer);
    void setDefaultColors(const QColor &fg, const QColor &bg);

private:
    ScreenCell currentStyle_;

    QColor defaultFg_{Qt::white};
    QColor defaultBg_{Qt::black};

    int savedRow_{0};
    int savedCol_{0};
    ScreenCell savedStyle_;

    void handleCSI(const QByteArray &seq, char final, ScreenBuffer &buf);
    void handleOSC(const QByteArray &seq);
    void applySGR(const QList<int> &codes);
    void flushText(QByteArray &plain, ScreenBuffer &buf);

    int         param(const QByteArray &seq, int index, int def = 1) const;
    static QColor xterm256Color(int index);
};