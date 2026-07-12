#include "ScreenRenderer.h"

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
