#include <QKeyEvent>
#include <QTextCursor>

#include "ShellEdit.h"

ShellEdit::ShellEdit(QWidget *parent)
    : QPlainTextEdit(parent)
{

}

void ShellEdit::keyPressEvent(QKeyEvent *event)
{
    QByteArray data;

    switch (event->key())
    {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        data = "\r";
        break;

    case Qt::Key_Backspace:
        data = "\b";
        break;

    default:
        data = event->text().toUtf8();
        break;
    }

    if (!data.isEmpty())
        emit bytesTyped(data);

    event->accept();
}