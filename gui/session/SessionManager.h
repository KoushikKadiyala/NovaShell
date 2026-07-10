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

private:
    QStackedWidget *stack_;
};