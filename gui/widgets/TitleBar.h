#pragma once

#include <QWidget>
#include <QPoint>

class QLabel;
class QWindow;
class ThemeSelector;
class NavigationButtons;

class TitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget *parent = nullptr);

private:

    ThemeSelector *themeSelector;

    NavigationButtons *closeButton;
    NavigationButtons *minimizeButton;
    NavigationButtons *maximizeButton;

protected:
    void mousePressEvent(QMouseEvent *event) override;
};