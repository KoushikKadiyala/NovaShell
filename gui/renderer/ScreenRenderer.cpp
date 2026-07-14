#include "ScreenRenderer.h"

#include <QColor>
#include <QList>
#include <QApplication>

ScreenRenderer::ScreenRenderer()
{
    currentStyle_.fg   = Qt::white;
    currentStyle_.bg   = Qt::black;
    currentStyle_.bold = false;
}

void ScreenRenderer::setDefaultColors(const QColor &fg, const QColor &bg)
{
    defaultFg_ = fg;
    defaultBg_ = bg;
    currentStyle_.fg = fg;
    currentStyle_.bg = bg;
}


// -----------------------------------------------------------------------
// Main render loop
// -----------------------------------------------------------------------

void ScreenRenderer::render(const QByteArray &data, ScreenBuffer &buffer)
{
    int i = 0;
    QByteArray plain;

    while (i < data.size())
    {
        const unsigned char ch = static_cast<unsigned char>(data[i]);

        // --- C0 control characters ---
        switch (ch)
        {
        case '\r':
            flushText(plain, buffer);
            buffer.carriageReturn();
            ++i; continue;

        case '\n':
        case '\v':
        case '\f':
            flushText(plain, buffer);
            buffer.lineFeed();
            buffer.carriageReturn();
            ++i; continue;

        case '\b':
            flushText(plain, buffer);
            buffer.moveLeft();
            ++i; continue;

        case '\t':
        {
            flushText(plain, buffer);
            const int col  = buffer.column();
            const int next = (col + 8) & ~7;
            for (int s = col; s < next; ++s)
            {
                ScreenCell cell = currentStyle_;
                cell.ch = ' ';
                buffer.put(cell);
            }
            ++i; continue;
        }

        case '\a':
            flushText(plain, buffer);
            QApplication::beep();
            ++i; continue;

        case 0x0e: // SO
        case 0x0f: // SI
            ++i; continue;
        case 0x1b:
            break;

        default:
            if (ch < 0x20) { ++i; continue; } // swallow other C0
            break;
        }

        // --- ESC ---
        if(ch == 0x1b)
        {

        if(handleEscape(i,data,plain,buffer))
            continue;
        }
        // --- printable text ---
        plain.append(data[i++]);
    }

    flushText(plain, buffer);
}
// -----------------------------------------------------------------------
// xterm 256-colour palette
// -----------------------------------------------------------------------

QColor ScreenRenderer::xterm256Color(int index)
{
    static const QColor ansi16[16] = {
        Qt::black,           QColor("#ff5555"),
        QColor("#50fa7b"),   QColor("#f1fa8c"),
        QColor("#6272a4"),   QColor("#ff79c6"),
        QColor("#8be9fd"),   Qt::white,
        QColor("#6272a4"),   QColor("#ff6e6e"),
        QColor("#69ff94"),   QColor("#ffffa5"),
        QColor("#8be9fd"),   QColor("#ff92df"),
        QColor("#a4ffff"),   Qt::white,
    };

    if (index < 16) return ansi16[index];

    if (index < 232)
    {
        const int v = index - 16;
        const int b = v % 6;
        const int g = (v / 6) % 6;
        const int r = v / 36;
        auto c = [](int n){ return n == 0 ? 0 : 55 + n * 40; };
        return QColor(c(r), c(g), c(b));
    }

    const int gray = 8 + (index - 232) * 10;
    return QColor(gray, gray, gray);
}

