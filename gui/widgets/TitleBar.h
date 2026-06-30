#pragma once

#include <QWidget>
#include <QPoint>

class QLabel;
class ThemeSelector;
class NavigationButtons;

class TitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget *parent = nullptr);

private:

    ThemeSelector *themeSelector;

    QPoint dragPosition;
    bool dragging{false};

    NavigationButtons *closeButton;
    NavigationButtons *minimizeButton;
    NavigationButtons *maximizeButton;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
};