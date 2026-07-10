#pragma once

#include <QWidget>
#include <QVector>

class QHBoxLayout;
class QPushButton;
class TabButton;

class TabBar : public QWidget
{
    Q_OBJECT

public:
    explicit TabBar(QWidget *parent = nullptr);

    int addTab(const QString &title);

    void setCurrentTab(int index);

    int currentTab() const;

signals:
    void currentChanged(int index);
    void newTabRequested();

private:
    QVector<TabButton*> tabs_;

    QHBoxLayout *layout_;
    QPushButton *addButton_;

    int currentIndex_ = -1;
};