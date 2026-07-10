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
    void dragStrated(const QPoint &globalPos);
    void dragged(const QPoint &globalPos);
    void dragFinished();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    // void mouseMoveEvent(QMouseEvent *event) override;
    // void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QLabel *label_;
    QPushButton *closeBtn_;
    bool         active_ = false;

    void updateStyle();
};