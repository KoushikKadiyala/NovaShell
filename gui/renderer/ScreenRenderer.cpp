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

int ScreenRenderer::param(const QByteArray &seq, int index, int def) const
{
    const QList<QByteArray> parts = seq.split(';');
    if (index >= parts.size()) return def;
    bool ok;
    const int v = parts[index].toInt(&ok);
    return (ok && v > 0) ? v : def;
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
        if (ch == 0x1b)
        {   
            flushText(plain, buffer);
            if (i + 1 >= data.size()) break;

            const unsigned char next =
                static_cast<unsigned char>(data[i + 1]);

            // CSI
            if (next == '[')
            {
                i += 2;
                QByteArray seq;

                while (i < data.size() &&
                       !((unsigned char)data[i] >= 0x40 &&
                         (unsigned char)data[i] <= 0x7e))
                    seq.append(data[i++]);

                if (i >= data.size()) break;
                handleCSI(seq, data[i++], buffer);
                continue;
            }

            // OSC
            if (next == ']')
            {
                i += 2;
                QByteArray osc;
                while (i < data.size())
                {
                    if ((unsigned char)data[i] == 0x07)
                        { ++i; break; }
                    if ((unsigned char)data[i] == 0x1b &&
                        i + 1 < data.size() && data[i+1] == '\\')
                        { i += 2; break; }
                    osc.append(data[i++]);
                }
                handleOSC(osc);
                continue;
            }

            // DCS / PM / APC — skip until ST
            if (next == 'P' || next == '^' || next == '_')
            {
                i += 2;
                while (i + 1 < data.size() &&
                       !((unsigned char)data[i] == 0x1b &&
                         data[i+1] == '\\'))
                    ++i;
                if (i + 1 < data.size()) i += 2;
                continue;
            }

            // Fe sequences
            switch (next)
            {
            case '7': // DECSC save cursor
                savedRow_   = buffer.row();
                savedCol_   = buffer.column();
                savedStyle_ = currentStyle_;
                break;
            case '8': // DECRC restore cursor
                buffer.setCursorPosition(savedRow_, savedCol_);
                currentStyle_ = savedStyle_;
                break;
            case 'M': buffer.reverseLineFeed();    break; // RI
            case 'D': buffer.lineFeed();           break; // IND
            case 'E': buffer.lineFeed();                  // NEL
                      buffer.carriageReturn();     break;
            case 'c': buffer.reset();                     // RIS
                currentStyle_.fg   = defaultFg_;
                currentStyle_.bg   = defaultBg_;
                currentStyle_.bold = false;
                break;
            case '=': // DECPAM
            case '>': // DECPNM
                break;
            case '(': case ')': case '*': case '+':
                ++i; // skip charset designator
                break;
            case 'H': break; // HTS
            default:  break;
            }

            i += 2;
            continue;
        }

        // --- printable text ---
        plain.append(data[i++]);
    }

    flushText(plain, buffer);
}

// -----------------------------------------------------------------------
// CSI dispatcher
// -----------------------------------------------------------------------

void ScreenRenderer::handleCSI(const QByteArray &seq,
                                char final,
                                ScreenBuffer &buf)
{   
    // Private modes
    if (seq.startsWith('?'))
    {
        const QByteArray priv = seq.mid(1);
        const bool on = (final == 'h');

        if      (priv == "1")    buf.setApplicationCursorKeys(on);
        else if (priv == "7")    buf.setAutoWrap(on);
        else if (priv == "12")   buf.setCursorBlinking(on);
        else if (priv == "25")   buf.setCursorVisible(on);
        else if (priv == "47" || priv == "1047")
        {
            on ? buf.enterAlternateScreen() : buf.leaveAlternateScreen();
        }
        else if (priv == "1049")
        {
            on ? buf.saveCursorAndEnterAlternate()
               : buf.restoreCursorAndLeaveAlternate();
        }
        else if (priv == "2004") { /* bracketed paste — ignore */ }

        if (final == 's') buf.saveCursor();
        if (final == 'r') buf.restoreCursor();
        return;
    }

    // SGR
    if (final == 'm')
    {
        QList<int> codes;
        if (seq.isEmpty())
            codes.append(0);
        else
            for (const QByteArray &p : seq.split(';'))
            {
                bool ok;
                const int v = p.toInt(&ok);
                if (ok) codes.append(v);
            }
        applySGR(codes);
        return;
    }

    switch (final)
    {
    // --- cursor movement ---
    case 'A': buf.moveCursorUp(param(seq, 0));    break; // CUU
    case 'B': buf.moveCursorDown(param(seq, 0));  break; // CUD
    case 'C': buf.moveRight(param(seq, 0));       break; // CUF
    case 'D': buf.moveLeft(param(seq, 0));        break; // CUB
    case 'E':                                            // CNL
        buf.moveCursorDown(param(seq, 0));
        buf.carriageReturn();
        break;
    case 'F':                                            // CPL
        buf.moveCursorUp(param(seq, 0));
        buf.carriageReturn();
        break;
    case 'G': buf.setCursorColumn(param(seq, 0) - 1); break; // CHA
    case 'H':                                                  // CUP
    case 'f':                                                  // HVP
    {
        const int r = param(seq, 0) - 1;
        const int c = param(seq, 1) - 1;
        buf.setCursorPosition(r, c);
        break;
    }
    case 'd': buf.setCursorRow(param(seq, 0) - 1);    break; // VPA

    // --- erase ---
    case 'J': buf.eraseDisplay(seq.isEmpty() ? 0 : seq.toInt()); break;
    case 'K':
    {
        const int mode = seq.isEmpty() ? 0 : seq.toInt();
        if      (mode == 0) buf.eraseToEndOfLine();
        else if (mode == 1) buf.eraseToStartOfLine();
        else if (mode == 2) buf.eraseEntireLine();
        break;
    }
    case 'X': buf.eraseChars(seq.isEmpty() ? 1 : seq.toInt()); break; // ECH

    // --- insert / delete ---
    case 'P': buf.deleteChars(seq.isEmpty() ? 1 : seq.toInt()); break; // DCH
    case '@': buf.insertChars(seq.isEmpty() ? 1 : seq.toInt()); break; // ICH
    case 'M': buf.deleteLines(seq.isEmpty() ? 1 : seq.toInt()); break; // DL
    case 'L': buf.insertLines(seq.isEmpty() ? 1 : seq.toInt()); break; // IL

    // --- scroll ---
    case 'S': buf.scrollUpN(seq.isEmpty()   ? 1 : seq.toInt()); break; // SU
    case 'T': buf.scrollDownN(seq.isEmpty() ? 1 : seq.toInt()); break; // SD

    // --- scroll region ---
    case 'r':
    {
        const int top    = param(seq, 0) - 1;
        const int bottom = param(seq, 1) - 1;
        buf.setScrollRegion(top, bottom);
        break;
    }

    // --- cursor save/restore ---
    case 's': buf.saveCursor();    break; // SCOSC
    case 'u': buf.restoreCursor(); break; // SCORC

    // --- tab ---
    case 'I': // CHT forward N tabs
    {
        const int n = seq.isEmpty() ? 1 : seq.toInt();
        for (int t = 0; t < n; ++t)
        {
            const int col  = buf.column();
            const int next = (col + 8) & ~7;
            buf.setCursorColumn(std::min(next, buf.screenCols() - 1));
        }
        break;
    }
    case 'Z': // CBT backward N tabs
    {
        const int n = seq.isEmpty() ? 1 : seq.toInt();
        for (int t = 0; t < n; ++t)
        {
            const int col  = buf.column();
            const int prev = ((col - 1) & ~7);
            buf.setCursorColumn(std::max(prev, 0));
        }
        break;
    }

    // --- device/mode ---
    case 'n': break; // DSR — cursor position report (ignore)
    case 'c': break; // DA  — device attributes (ignore)
    case 'h': break; // SM  — mode set (public, ignore)
    case 'l': break; // RM  — mode reset (public, ignore)

    default: break;
    }
}

// -----------------------------------------------------------------------
// OSC
// -----------------------------------------------------------------------

void ScreenRenderer::handleOSC(const QByteArray &seq)
{
    Q_UNUSED(seq); // window title, colour palette etc — silently ignored
}

// -----------------------------------------------------------------------
// SGR
// -----------------------------------------------------------------------

void ScreenRenderer::applySGR(const QList<int> &codes)
{
    int i = 0;
    while (i < codes.size())
    {
        const int code = codes[i++];
        switch (code)
        {
        case 0:
            currentStyle_.fg   = defaultFg_;
            currentStyle_.bg   = defaultBg_;
            currentStyle_.bold = false;
            break;

        case 1:  currentStyle_.bold = true;  break;
        case 2:  break; // dim
        case 3:  break; // italic
        case 4:  break; // underline
        case 5:  break; // blink slow
        case 6:  break; // blink fast
        case 7:  // reverse
        {
            QColor tmp       = currentStyle_.fg;
            currentStyle_.fg = currentStyle_.bg;
            currentStyle_.bg = tmp;
            break;
        }
        case 8:  break; // conceal
        case 9:  break; // strikethrough
        case 21: break; // double underline
        case 22: currentStyle_.bold = false; break;
        case 23: break; // italic off
        case 24: break; // underline off
        case 25: break; // blink off
        case 27: // reverse off
        {
            QColor tmp       = currentStyle_.fg;
            currentStyle_.fg = currentStyle_.bg;
            currentStyle_.bg = tmp;
            break;
        }
        case 28: break; // conceal off
        case 29: break; // strikethrough off

        // normal foreground
        case 30: currentStyle_.fg = Qt::black;           break;
        case 31: currentStyle_.fg = QColor("#ff5555");   break;
        case 32: currentStyle_.fg = QColor("#50fa7b");   break;
        case 33: currentStyle_.fg = QColor("#f1fa8c");   break;
        case 34: currentStyle_.fg = QColor("#6272a4");   break;
        case 35: currentStyle_.fg = QColor("#ff79c6");   break;
        case 36: currentStyle_.fg = QColor("#8be9fd");   break;
        case 37: currentStyle_.fg = Qt::white;           break;
        case 38: // 256 / truecolor fg
            if (i < codes.size() && codes[i] == 5 &&
                i + 1 < codes.size())
            {
                currentStyle_.fg = xterm256Color(codes[i+1]);
                i += 2;
            }
            else if (i < codes.size() && codes[i] == 2 &&
                     i + 3 < codes.size())
            {
                currentStyle_.fg = QColor(codes[i+1], codes[i+2], codes[i+3]);
                i += 4;
            }
            break;
        case 39: currentStyle_.fg = defaultFg_; break;

        // normal background
        case 40: currentStyle_.bg = Qt::black;           break;
        case 41: currentStyle_.bg = QColor("#ff5555");   break;
        case 42: currentStyle_.bg = QColor("#50fa7b");   break;
        case 43: currentStyle_.bg = QColor("#f1fa8c");   break;
        case 44: currentStyle_.bg = QColor("#6272a4");   break;
        case 45: currentStyle_.bg = QColor("#ff79c6");   break;
        case 46: currentStyle_.bg = QColor("#8be9fd");   break;
        case 47: currentStyle_.bg = Qt::white;           break;
        case 48: // 256 / truecolor bg
            if (i < codes.size() && codes[i] == 5 &&
                i + 1 < codes.size())
            {
                currentStyle_.bg = xterm256Color(codes[i+1]);
                i += 2;
            }
            else if (i < codes.size() && codes[i] == 2 &&
                     i + 3 < codes.size())
            {
                currentStyle_.bg = QColor(codes[i+1], codes[i+2], codes[i+3]);
                i += 4;
            }
            break;
        case 49: currentStyle_.bg = defaultBg_; break;

        // bright foreground
        case 90: currentStyle_.fg = QColor("#6272a4");   break;
        case 91: currentStyle_.fg = QColor("#ff6e6e");   break;
        case 92: currentStyle_.fg = QColor("#69ff94");   break;
        case 93: currentStyle_.fg = QColor("#ffffa5");   break;
        case 94: currentStyle_.fg = QColor("#8be9fd");   break;
        case 95: currentStyle_.fg = QColor("#ff92df");   break;
        case 96: currentStyle_.fg = QColor("#a4ffff");   break;
        case 97: currentStyle_.fg = Qt::white;           break;

        // bright background
        case 100: currentStyle_.bg = QColor("#6272a4");  break;
        case 101: currentStyle_.bg = QColor("#ff5555");  break;
        case 102: currentStyle_.bg = QColor("#50fa7b");  break;
        case 103: currentStyle_.bg = QColor("#f1fa8c");  break;
        case 104: currentStyle_.bg = QColor("#8be9fd");  break;
        case 105: currentStyle_.bg = QColor("#ff79c6");  break;
        case 106: currentStyle_.bg = QColor("#8be9fd");  break;
        case 107: currentStyle_.bg = Qt::white;          break;

        default: break;
        }
    }
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

// -----------------------------------------------------------------------
// Flush plain text
// -----------------------------------------------------------------------

void ScreenRenderer::flushText(QByteArray &plain, ScreenBuffer &buf)
{
    if (plain.isEmpty()) return;

    const QString decoded = QString::fromUtf8(plain);
    for (const QChar ch : decoded)
    {
        ScreenCell cell = currentStyle_;
        cell.ch = ch;
        buf.put(cell);
    }
    plain.clear();
}