#ifndef SHELLVIEW_H
#define SHELLVIEW_H

#include <QWidget>
#include "ShellEdit.h"

class QPlainTextEdit;

class ShellView : public QWidget
{
    Q_OBJECT

public:
    explicit ShellView(QWidget *parent = nullptr);

    void print(const QString &text);
    void printPrompt();

private slots:
    void executeCommand(const QString &command);
private:
    ShellEdit *shell;
    QByteArray m_CurrentCommandBuffer;
};

#endif