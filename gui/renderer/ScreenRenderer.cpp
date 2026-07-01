#include "ScreenRenderer.h"

#include <QColor>
#include <QList>

ScreenRenderer::ScreenRenderer()
{
    currentStyle.fg = Qt::white;
    currentStyle.bg = Qt::black;
    currentStyle.bold = false;
}

void ScreenRenderer::render(const QByteArray &data,
                            ScreenBuffer &buffer)
{
    int i = 0;
    QByteArray plainText;

    while (i < data.size())
    {
        // ---------- Carriage Return ----------
        if (data[i] == '\r')
        {
            flushText(plainText, buffer);

            buffer.carriageReturn();

            ++i;
            continue;
        }

        // ---------- Line Feed ----------
        if (data[i] == '\n')
        {
            flushText(plainText, buffer);

            buffer.lineFeed();
            buffer.carriageReturn();

            ++i;
            continue;
        }

        // ---------- ANSI CSI ----------
        if (data[i] == '\x1b' &&
            i + 1 < data.size() &&
            data[i + 1] == '[')
        {
            flushText(plainText, buffer);

            i += 2;

            QByteArray sequence;

            while (i < data.size() &&
                   !(data[i] >= '@' && data[i] <= '~'))
            {
                sequence.append(data[i]);
                ++i;
            }

            if (i >= data.size())
                break;

            char final = data[i++];

            switch (final)
            {
            case 'm':
            {
                QList<int> codes;

                for (const QByteArray &part : sequence.split(';'))
                {
                    bool ok;
                    int value = part.toInt(&ok);

                    if (ok)
                        codes.append(value);
                }

                applySGR(codes);
                break;
            }

            case 'C':
            {
                int count = sequence.isEmpty() ? 1 : sequence.toInt();
                buffer.moveRight(count);
                break;
            }

            case 'D':
            {
                int count = sequence.isEmpty() ? 1 : sequence.toInt();
                buffer.moveLeft(count);
                break;
            }

            case 'K':
            {
                buffer.eraseToEndOfLine();
                break;
            }

            default:
                // Ignore unsupported CSI commands
                break;
            }

            continue;
        }

        // ---------- Normal text ----------
        plainText.append(data[i]);
        ++i;
    }

    flushText(plainText, buffer);
}

void ScreenRenderer::applySGR(const QList<int> &codes)
{
    for (int code : codes)
    {
        switch (code)
        {
        case 0:
            currentStyle.fg = Qt::white;
            currentStyle.bg = Qt::black;
            currentStyle.bold = false;
            break;

        case 1:
            currentStyle.bold = true;
            break;

        case 22:
            currentStyle.bold = false;
            break;

        case 30: currentStyle.fg = Qt::black; break;
        case 31: currentStyle.fg = QColor("#ff5555"); break;
        case 32: currentStyle.fg = QColor("#50fa7b"); break;
        case 33: currentStyle.fg = QColor("#f1fa8c"); break;
        case 34: currentStyle.fg = QColor("#6272a4"); break;
        case 35: currentStyle.fg = QColor("#ff79c6"); break;
        case 36: currentStyle.fg = QColor("#8be9fd"); break;
        case 37: currentStyle.fg = Qt::white; break;

        case 90: currentStyle.fg = QColor("#6272a4"); break;
        case 91: currentStyle.fg = QColor("#ff6e6e"); break;
        case 92: currentStyle.fg = QColor("#69ff94"); break;
        case 93: currentStyle.fg = QColor("#ffffa5"); break;
        case 94: currentStyle.fg = QColor("#8be9fd"); break;
        case 95: currentStyle.fg = QColor("#ff92df"); break;
        case 96: currentStyle.fg = QColor("#8be9fd"); break;
        case 97: currentStyle.fg = Qt::white; break;
        }
    }
}

void ScreenRenderer::flushText(QByteArray &plainText,
                               ScreenBuffer &buffer)
{
    if (plainText.isEmpty())
        return;

    QString decoded = QString::fromUtf8(plainText);

    for (QChar ch : decoded)
    {
        ScreenCell cell = currentStyle;
        cell.ch = ch;

        buffer.put(cell);
    }

    plainText.clear();
}