#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void enableMouseTracking(QWidget *widget);
    bool eventFilter(QObject *obj,QEvent *event) override;
private:
    static constexpr int RESIZE_MARGIN = 8;
    Qt::Edges hitTest(const QPoint &pos);
    void updateResizeCursor(const QPoint &pos);
};

#endif
