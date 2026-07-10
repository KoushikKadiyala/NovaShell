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

    void removeTab(int index);

signals:
    void currentChanged(int index);
    void newTabRequested();
    void tabCloseRequested(int index);
private:
    bool dragging_ = false;

    QPoint dragStartPos_;

    TabButton *draggedTab_ = nullptr;
    
    QVector<TabButton*> tabs_;

    QHBoxLayout *layout_;
    QPushButton *addButton_;

    int currentIndex_ = -1;
};