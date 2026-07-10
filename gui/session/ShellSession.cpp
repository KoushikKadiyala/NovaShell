#include "ShellSession.h"

#include <QVBoxLayout>

ShellSession::ShellSession(QWidget *parent)
    : QWidget(parent),
      buffer_(24, 80)
{
    screen_ = new ScreenWidget(&buffer_, this);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(screen_);

    connect(
        screen_,
        &ScreenWidget::bytesTyped,
        &pty_,
        &PtySession::writeData);

    connect(
        screen_,
        &ScreenWidget::terminalResized,
        &pty_,
        &PtySession::resize);

    connect(
        &pty_,
        &PtySession::dataReceived,
        this,
        &ShellSession::HandlePtyOutput);

    connect(&pty_,
            &PtySession::shellExited,
            this,
            [this](int code)
            {   Q_UNUSED(code);
                emit sessionEnded();
            });

    pty_.start();
}

void ShellSession::HandlePtyOutput(const QByteArray &data){
    renderer_.render(data, buffer_);
    refresh();
}
void ShellSession::refresh(){
    screen_->update();
}