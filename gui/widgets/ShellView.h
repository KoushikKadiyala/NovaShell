#pragma once

#include <QWidget>
#include <QByteArray>

class ShellEdit;

class ShellView : public QWidget
{
    Q_OBJECT

public:
    explicit ShellView(QWidget *parent = nullptr);

public slots:
    void insert(const QString &text);

signals:
    void bytesTyped(const QString &data);

private:
    ShellEdit *shell;
};