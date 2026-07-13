#include "ScreenRenderer.h"

bool ScreenRenderer::handleEscape(int &i,const QByteArray &data,QByteArray &plain,ScreenBuffer &buffer)
        {   
            flushText(plain, buffer);
            if (i + 1 >= data.size()) return false;

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

                if (i >= data.size()) return false;
                handleCSI(seq, data[i++], buffer);
                return true;
            }

            // OSC
            if (next == ']')
            {
                i += 2;
                QByteArray osc;
                while (i < data.size())
                {
                    if ((unsigned char)data[i] == 0x07)
                        { ++i; return true; }
                    if ((unsigned char)data[i] == 0x1b &&
                        i + 1 < data.size() && data[i+1] == '\\')
                        { i += 2; return true; }
                    osc.append(data[i++]);
                }
                handleOSC(osc);
                return true;
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
                return true;
            }

            // Fe sequences
            switch (next)
            {
            case '7': // DECSC save cursor
                savedRow_   = buffer.row();
                savedCol_   = buffer.column();
                savedStyle_ = currentStyle_;
                return true;
            case '8': // DECRC restore cursor
                buffer.setCursorPosition(savedRow_, savedCol_);
                currentStyle_ = savedStyle_;
                return true;
            case 'M': buffer.reverseLineFeed();    return true; // RI
            case 'D': buffer.lineFeed();           return true; // IND
            case 'E': buffer.lineFeed();                  // NEL
                      buffer.carriageReturn();     return true;
            case 'c': buffer.reset();                     // RIS
                currentStyle_.fg   = defaultFg_;
                currentStyle_.bg   = defaultBg_;
                currentStyle_.bold = false;
                return true;
            case '=': // DECPAM
            case '>': // DECPNM
                return true;
            case '(': case ')': case '*': case '+':
              if(i+2<data.size())
                i+=3;
              else
                i = data.size();
              return true;
            case 'H': return true; // HTS
            default:  return true;
            }

            i += 2;
            return true;
        }