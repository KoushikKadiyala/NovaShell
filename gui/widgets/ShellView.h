#pragma once

#include <QWidget>
#include <QByteArray>

#include "../renderer/ScreenRenderer.h"
#include "../screen/ScreenBuffer.h"
#include "../screen/ScreenWidget.h"


class ShellEdit;

class ShellView : public QWidget
{
    Q_OBJECT

public:
    explicit ShellView(QWidget *parent = nullptr);
    

public slots:
    void insert(const QByteArray &data);

signals:
    void bytesTyped(const QString &data);

private:
    ScreenWidget *screenWidget;
    ScreenRenderer renderer;
    ScreenBuffer Buffer;
};