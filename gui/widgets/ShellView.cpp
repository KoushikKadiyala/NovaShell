#include "ShellView.h"
#include "ShellEdit.h"

#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QFrame>
#include <QTextCursor>
#include <QTextOption>
#include <QDebug>

extern "C"{
#include "../../include/core.h"
#include "../../include/io.h"
}

static ShellView* currentView = nullptr;

static void guiOutput(const char* text)
{
    if (currentView)
        currentView->print(QString::fromUtf8(text));
}

ShellView::ShellView(QWidget *parent)
    : QWidget(parent)
{   shell = new ShellEdit(this);
    
    currentView = this;
    shell_set_output(guiOutput);

    connect(shell,
            &ShellEdit::commandEntered,
            this,
            &ShellView::executeCommand);
    
    //terminal appearrance
    shell->setFrameStyle(QFrame::NoFrame);
    shell->setUndoRedoEnabled(false);
    shell->setWordWrapMode(QTextOption::NoWrap);
    shell->setCenterOnScroll(true);
    shell->setAcceptDrops(false);
    
    //monospace font
    QFont font("JetBrains Mono", 12);
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    shell->setFont(font);

    //layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20,20,20,20);
    layout->addWidget(shell);

    //intial prompt
    printPrompt();
}

void ShellView::executeCommand(const QString &command){
    m_CurrentCommandBuffer = command.toUtf8();
    shell_execute(m_CurrentCommandBuffer.data());
    print("\n");
    printPrompt();
}
void ShellView::printPrompt(){
    print("~/NovaShell\n❯");
}

void ShellView::print(const QString &text){
    QTextCursor cursor = shell->textCursor();
    cursor.movePosition(QTextCursor::End);

    shell->setTextCursor(cursor);
    shell->insertPlainText(text);

    shell->ensureCursorVisible();
}