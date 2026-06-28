#include "ShellView.h"
#include "ShellEdit.h"

#include <QVBoxLayout>
#include <QFrame>
#include <QTextCursor>
#include <QTextOption>

ShellView::ShellView(QWidget *parent)
    : QWidget(parent)
{
    shell = new ShellEdit(this);

    connect(shell,
            &ShellEdit::bytesTyped,
            this,
            &ShellView::bytesTyped);

    // Terminal appearance
    shell->setFrameStyle(QFrame::NoFrame);
    shell->setUndoRedoEnabled(false);
    shell->setWordWrapMode(QTextOption::NoWrap);
    shell->setCenterOnScroll(true);
    shell->setAcceptDrops(false);

    // Monospace font
    QFont font("JetBrains Mono", 12);
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    shell->setFont(font);

    // Layout
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->addWidget(shell);
}

void ShellView::insert(const QString &text)
{
    QTextCursor cursor = shell->textCursor();

    cursor.insertText(QString(text));

    shell->setTextCursor(cursor);
    shell->ensureCursorVisible();
}