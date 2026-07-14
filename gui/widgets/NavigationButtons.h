#pragma once

#include <QPushButton>
#include <QColor>

class NavigationButtons : public QPushButton
{
    Q_OBJECT

public:
    explicit NavigationButtons(const QColor &color,
                                QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QColor m_color;
};