#pragma once

#include <QWidget>
#include <QStackedWidget>

class ShellSession;

class SessionManager : public QWidget
{
    Q_OBJECT

public:
    explicit SessionManager(QWidget *parent = nullptr);

    ShellSession *createSession();

    void setCurrentSession(int index);

    ShellSession *currentSession() const;

    int currentIndex() const;

    int count() const;

    int indexOf(ShellSession *session) const;
    
    void moveSession(int from,int to);
    void removeSession(int index);
private:
    QStackedWidget *stack_;
};