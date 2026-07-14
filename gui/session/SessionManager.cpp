#include "SessionManager.h"
#include "ShellSession.h"

#include <QVBoxLayout>

SessionManager::SessionManager(QWidget *parent)
    : QWidget(parent)
{
    stack_ = new QStackedWidget(this);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    layout->addWidget(stack_);
}
ShellSession *SessionManager::createSession()
{
    auto *session = new ShellSession(stack_);

    stack_->addWidget(session);
    stack_->setCurrentWidget(session);
    
    return session;
}

ShellSession *SessionManager::currentSession() const{
    return qobject_cast<ShellSession*>(stack_->currentWidget());
}

int SessionManager::currentIndex()const{
    return stack_->currentIndex();
}

void SessionManager::setCurrentSession(int index){
    stack_->setCurrentIndex(index);
    if(auto *s = currentSession())
        s->setFocus();
}

int SessionManager::count() const{
    return stack_->count();
}

int SessionManager::indexOf(ShellSession *session) const{
    return stack_->indexOf(session);
}

void SessionManager::removeSession(int index){
    QWidget *widget = stack_->widget(index);
    if(!widget)
        return;
    stack_->removeWidget(widget);
    widget->deleteLater();
}

void SessionManager::moveSession(int from,int to)
{
    if(from == to)
        return;
    if(from<0 || from >= stack_->count())
        return;
    if(to<0 || to >= stack_->count())
        return;
    QWidget *page = stack_->widget(from);

    stack_->removeWidget(page);

    stack_->insertWidget(to,page);
    stack_->setCurrentWidget(page);

}