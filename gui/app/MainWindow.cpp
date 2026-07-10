#include "MainWindow.h"
#include "../session/SessionManager.h"
#include "../session/ShellSession.h"
#include "../widgets/TitleBar.h"
#include "../widgets/TabBar.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QWindow>
#include <QApplication>

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
   tabBar_ = new TabBar(container);
   manager_ = new SessionManager(container);

    connect(tabBar_, &TabBar::currentChanged,
            manager_, &SessionManager::setCurrentSession);
    
    connect(tabBar_,&TabBar::newTabRequested,
            this, &MainWindow::addTab);
    connect(tabBar_,&TabBar::tabCloseRequested,
            this, &MainWindow::removeTab);
    connect(tabBar_, &TabBar::tabMoved,
            manager_, &SessionManager::moveSession);

    layout->addWidget(titleBar);
    layout->addWidget(tabBar_);
    layout->addWidget(manager_);

    container->setLayout(layout);
    setCentralWidget(container);

    setMouseTracking(true);
    enableMouseTracking(this);
    qApp->installEventFilter(this);

    addTab();
}
void MainWindow::addTab(){
    ShellSession *session = manager_ ->createSession();

    int index = tabBar_->addTab(QString("shell %1").arg(manager_->count()));

    tabBar_->setCurrentTab(index);
    connect(session, &ShellSession::sessionEnded,
            this,[this,session](){
                int idx = manager_ ->indexOf(session);
                if (idx != -1)
                    removeTab(idx);
            });

}

void MainWindow::removeTab(int index)
{
    tabBar_->removeTab(index);
    manager_->removeSession(index);
    if (manager_ -> count()==0)
        addTab();
}
Qt::Edges MainWindow::hitTest(const QPoint &pos)
{
    Qt::Edges edges;

    if (pos.x() <= RESIZE_MARGIN)
        edges |= Qt::LeftEdge;
    else if (pos.x() >= width() - RESIZE_MARGIN)
        edges |= Qt::RightEdge;
    else if (pos.y() >= height() - RESIZE_MARGIN)
        edges |= Qt::BottomEdge;

    return edges;
}

void MainWindow::updateResizeCursor(const QPoint &pos)
{
    Qt::Edges edges = hitTest(pos);

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
void MainWindow::enableMouseTracking(QWidget *widget){
    widget->setMouseTracking(true);
    for(QObject *obj : widget->children())
    {
        if(QWidget *child = qobject_cast<QWidget*>(obj))
            enableMouseTracking(child);
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseMove)
    {
        auto *mouse = static_cast<QMouseEvent*>(event);

        QPoint p = mapFromGlobal(mouse->globalPosition().toPoint());

        updateResizeCursor(p);
    }

    return QMainWindow::eventFilter(obj, event);
}