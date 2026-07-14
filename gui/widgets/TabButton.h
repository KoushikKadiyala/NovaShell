#pragma once

#include <QWidget>

class QPushButton;
class QLabel;

class TabButton : public QWidget
{
    Q_OBJECT

public:
    explicit TabButton(const QString &title, QWidget *parent = nullptr);

    void setActive(bool active);

signals:
    void clicked();
    void closeRequested();
    void middleClicked();

    void dragStrated(const QPoint &globalPos);
    void dragged(const QPoint &globalPos);
    void dragFinished();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QLabel *label_;
    QPushButton *closeBtn_;
    QPoint dragStartPos_;
    bool active_ = false;
    bool dragging_ = false;

    void updateStyle();
};