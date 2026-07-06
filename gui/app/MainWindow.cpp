#include "MainWindow.h"
#include "../widgets/ShellView.h"
#include "../pty/PtySession.h"
#include "../widgets/TitleBar.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QWindow>

MainWindow::MainWindow()
{  setAttribute(Qt::WA_TranslucentBackground);
   setWindowFlags(Qt::FramelessWindowHint);
   resize(1200,800);

    QWidget *container = new QWidget(this);
    container->setObjectName("windowContainer");
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
    connect(shell,
            &ShellView::terminalResized,
            pty,
            &PtySession::resize);
    pty->start();

    setCentralWidget(container);
    layout->addWidget(titleBar);
    layout->addWidget(shell);
    setMouseTracking(true);
    centralWidget()->setMouseTracking(true);

    container->setLayout(layout);
}
Qt::Edges MainWindow::hitTest(const QPoint &pos)
{
    Qt::Edges edges;

    if (pos.x() < RESIZE_MARGIN)
        edges |= Qt::LeftEdge;
    else if (pos.x() > width() - RESIZE_MARGIN)
        edges |= Qt::RightEdge;

    if (pos.y() < RESIZE_MARGIN)
        edges |= Qt::TopEdge;
    else if (pos.y() > height() - RESIZE_MARGIN)
        edges |= Qt::BottomEdge;

    return edges;
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    Qt::Edges edges = hitTest(event->pos());

    if (edges == (Qt::LeftEdge | Qt::TopEdge) ||
        edges == (Qt::RightEdge | Qt::BottomEdge))
    {
        setCursor(Qt::SizeFDiagCursor);
    }
    else if (edges == (Qt::RightEdge | Qt::TopEdge) ||
             edges == (Qt::LeftEdge | Qt::BottomEdge))
    {
        setCursor(Qt::SizeBDiagCursor);
    }
    else if (edges & (Qt::LeftEdge | Qt::RightEdge))
    {
        setCursor(Qt::SizeHorCursor);
    }
    else if (edges & (Qt::TopEdge | Qt::BottomEdge))
    {
        setCursor(Qt::SizeVerCursor);
    }
    else
    {
        unsetCursor();
    }
}
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    Qt::Edges edges = hitTest(event->pos());

    if (!edges)
        return;

    if (QWindow *w = windowHandle())
    {
        w->startSystemResize(edges);
        event->accept();
    }
}
void MainWindow::leaveEvent(QEvent *)
{
    unsetCursor();
}