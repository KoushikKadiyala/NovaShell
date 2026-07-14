#include "ScreenRenderer.h"

int ScreenRenderer::param(const QByteArray &seq, int index, int def) const
{
    const QList<QByteArray> parts = seq.split(';');
    if (index >= parts.size()) return def;
    bool ok;
    const int v = parts[index].toInt(&ok);
    return (ok && v > 0) ? v : def;
}

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