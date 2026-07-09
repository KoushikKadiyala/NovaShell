#pragma once

#include <QWidget>

#include "../pty/PtySession.h"
#include "../screen/ScreenBuffer.h"
#include "../renderer/ScreenRenderer.h"
#include "../screen/ScreenWidget.h"

class ShellSession : public QWidget
{
    Q_OBJECT

public:
    explicit ShellSession(QWidget *parent = nullptr);

private:
    PtySession pty_;
    ScreenBuffer buffer_;
    ScreenRenderer renderer_;
    ScreenWidget *screen_;
};