#include "MainWindow.h"
#include "../widgets/ShellView.h"
#include "../pty/PtySession.h"
#include "../widgets/TitleBar.h"

#include <QWidget>
#include <QVBoxLayout>

MainWindow::MainWindow()
{
   setWindowFlags(Qt::FramelessWindowHint);
   resize(1200,800);

    QWidget *container = new QWidget(this);
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

   auto *titleBar = new TitleBar(container);

    ShellView *shell = new ShellView(container);
    PtySession *pty = new PtySession(this);
    connect(pty,
    &PtySession::dataReceived,
    shell,
    &ShellView::insert );

    connect(shell,
    &ShellView::bytesTyped,
    pty,
    [pty](const QString &command){
        pty->writeData(command.toUtf8());
    });
    connect(pty,
            &PtySession::shellExited,
            this,
            [](int code){qDebug() << "shell exited:"<< code;});
    pty->start();


    layout->addWidget(titleBar);
    layout->addWidget(shell);

    container->setLayout(layout);
    setCentralWidget(container);
}
