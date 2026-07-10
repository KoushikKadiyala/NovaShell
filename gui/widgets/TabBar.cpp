#include "TabBar.h"
#include "TabButton.h"

#include <QHBoxLayout>
#include <QPushButton>

TabBar::TabBar(QWidget *parent)
    : QWidget(parent)
{
    layout_ = new QHBoxLayout(this);

    layout_->setContentsMargins(8, 0, 8, 0);
    layout_->setSpacing(2);

    addButton_ = new QPushButton("+", this);

    addButton_->setFixedSize(28, 28);
    addButton_->setFocusPolicy(Qt::NoFocus);

    layout_->addStretch();
    layout_->addWidget(addButton_);

    connect(addButton_,
            &QPushButton::clicked,
            this,
            &TabBar::newTabRequested);
}

int TabBar::addTab(const QString &title)
{
    auto *tab = new TabButton(title, this);

    layout_->insertWidget(layout_->count() - 2, tab);

    tabs_.push_back(tab);

    const int index = tabs_.size() - 1;

    connect(tab,
            &QPushButton::clicked,
            this,
            [this, index]()
            {
                setCurrentTab(index);
                emit currentChanged(index);
            });

    if (currentIndex_ == -1)
        setCurrentTab(0);

    return index;
}

void TabBar::setCurrentTab(int index)
{
    if (index < 0 || index >= tabs_.size())
        return;
    qDebug()<<"current tab ="<< index;
    currentIndex_ = index;

    for (int i = 0; i < tabs_.size(); ++i)
        tabs_[i]->setActive(i == currentIndex_);
}

int TabBar::currentTab() const
{
    return currentIndex_;
}