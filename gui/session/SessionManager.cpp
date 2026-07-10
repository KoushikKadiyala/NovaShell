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
    auto *session = new ShellSession;

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
}