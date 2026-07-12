#include "ScreenRenderer.h"

#include <QColor>
#include <QList>
#include <QApplication>

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

        // ---------- Backspace ----------
        if (data[i] == '\b')
        {
            flushText(plainText, buffer);
            buffer.moveLeft();
            ++i;
            continue;
        }

        // --------- Tab ---------------
        if (data[i] == '\t')
        {
            i++;
            continue;
        }

        // --------- BEL --------------
        if(data[i] == '\a')
        {
            flushText(plainText,buffer);
            QApplication::beep();
            ++i;
            continue;
        }
        // ---------- ANSI CSI ----------
if (data[i] == '\x1b')
{
    flushText(plainText, buffer);

    if (i + 1 >= data.size())
        break;

    char introducer = data[i + 1];

    // ---------------- CSI ----------------
    if (introducer == '[')
    {
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
        qDebug()<< "csi:"<< sequence << final;

        if (sequence == "?1049")
        {
            if (final == 'h')
                buffer.enterAlternateScreen();
            else if (final == 'l')
                buffer.leaveAlternateScreen();

            continue;
        }
        if(sequence == "?25")
        {
            if(final == 'h')
            buffer.setCursorVisible(true);
        else
            buffer.setCursorVisible(false);

        continue;
        }

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
            buffer.moveRight(sequence.isEmpty() ? 1 : sequence.toInt());
            break;

        case 'D':
            buffer.moveLeft(sequence.isEmpty() ? 1 : sequence.toInt());
            break;

        case 'K':
            buffer.eraseToEndOfLine();
            break;

        case 'P':
            buffer.deleteChars(sequence.isEmpty() ? 1 : sequence.toInt());
            break;

        case 'H':
        case 'f':
        {
            int row = 1;
            int col =1;

            QList<QByteArray> parts = sequence.split(';');

            if(parts.size()>=1 && !parts[0].isEmpty())
                row = parts[0].toInt();

            if(parts.size()>= 2 && !parts[1].isEmpty())
                col = parts[1].toInt();

            buffer.setCursorPosition(row-1,col-1);
            break;
        }
        case 'J':
        {
            int mode = sequence.isEmpty() ? 0 : sequence.toInt();

            buffer.eraseDisplay(mode);
            break;
        }
        case 'd':
        {
            int row = sequence.isEmpty() ? 1 : sequence.toInt();

            buffer.setCursorRow(row - 1);
        }
        case 'G':
        {
            int col = sequence.isEmpty() ? 1 : sequence.toInt();
            buffer.setCursorColumn(col - 1);

            break;
        }
        default:
            break;
        }

        continue;
    }

    // ---------------- Charset selection ----------------
    if ((introducer == '(' || introducer == ')') &&
        i + 2 < data.size())
    {
        // Ignore ESC(B, ESC)B, ESC(0, etc.
        i += 3;
        continue;
    }

    // ---------------- Unknown ESC ----------------
    ++i;
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