#include <QKeyEvent>
#include <QTextCursor>

#include "ShellEdit.h"

ShellEdit::ShellEdit(QWidget *parent)
    : QPlainTextEdit(parent)
{

}

void ShellEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return ||
        event->key() == Qt::Key_Enter)
    {
        QTextCursor cursor = textCursor();

        cursor.movePosition(QTextCursor::End);
        cursor.select(QTextCursor::LineUnderCursor);
        QString command = cursor.selectedText();
        command.remove("❯");
        command = command.trimmed();

        emit commandEntered(command);

        event->accept();
        return;
        }
    else{
    QPlainTextEdit::keyPressEvent(event);
    }
}