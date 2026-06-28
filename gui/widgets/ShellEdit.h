#ifndef SHELLEDIT_H
#define SHELLEDIT_H

#include <QtWidgets/QPlainTextEdit>
#include <QtGui/QKeyEvent>

class ShellEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit ShellEdit(QWidget *parent = nullptr);

signals:
    void bytesTyped(const QString &data);

protected:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif