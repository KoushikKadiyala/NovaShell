#include <QKeyEvent>
#include <QTextCursor>

#include "ShellEdit.h"

ShellEdit::ShellEdit(QWidget *parent)
    : QPlainTextEdit(parent)
{

}

void ShellEdit::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Return:
    case Qt::Key_Enter:
       emit bytesTyped("\r");
        event->accept();
        return;

    case Qt::Key_Backspace:
        emit bytesTyped("\b");
        event->accept();
        return;

    default:
        if (!event->text().isEmpty())
            emit bytesTyped(event->text().toUtf8());

        event->accept();
        return;
    }
}