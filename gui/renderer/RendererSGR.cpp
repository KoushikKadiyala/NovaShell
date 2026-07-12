#include "ScreenRenderer.h"

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
