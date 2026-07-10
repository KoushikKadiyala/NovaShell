#pragma once

#include <QPushButton>

class TabButton : public QPushButton
{
    Q_OBJECT
public:
    explicit TabButton(const QString &title, QWidget *parent = nullptr);

    void setActive(bool active);

private:
    bool active_ = false;
};