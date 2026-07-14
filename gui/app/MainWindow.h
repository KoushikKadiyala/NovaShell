#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class TabBar;
class SessionManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    bool eventFilter(QObject *obj,QEvent *event) override;
private:
    void enableMouseTracking(QWidget *widget);
    void removeTab(int index);
    void addTab();
    void nextTab();
    void previousTab();
    void setupConnections();
    void setupShortcuts();

    TabBar          *tabBar_{nullptr};
    SessionManager *manager_{nullptr};
    
    static constexpr int RESIZE_MARGIN = 8;
    Qt::Edges hitTest(const QPoint &pos);
    void updateResizeCursor(const QPoint &pos);
};

#endif
