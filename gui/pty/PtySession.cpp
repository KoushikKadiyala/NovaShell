#include "PtySession.h"

#include <QDebug>
#include <QSocketNotifier>
#include <QString>

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/wait.h>

bool PtySession::start(){
    masterFd = posix_openpt(O_RDWR | O_NOCTTY);

if (masterFd == -1)
{
    perror("posix_openpt");
    return false;
}
    if (grantpt(masterFd) == -1)
{
    perror("grantpt");
    return false;
}
if (unlockpt(masterFd) == -1)
{
    perror("unlockpt");
    return false;
}
char *slaveName = ptsname(masterFd);

if (slaveName == nullptr)
{
    perror("ptsname");
    return false;
}

childPid = fork();

if (childPid == -1)
{
    perror("fork");
    return false;
}

if (childPid == 0)
{
        if (setsid() == -1){
            perror("setsid");
            _exit(EXIT_FAILURE);
        }

    int slaveFd = open(slaveName, O_RDWR);
    if(slaveFd == -1){
        perror("open");
        _exit(EXIT_FAILURE);
    }

    // Redirect standard input
if (dup2(slaveFd, STDIN_FILENO) == -1)
{
    perror("dup2 stdin");
    _exit(EXIT_FAILURE);
}

// Redirect standard output
if (dup2(slaveFd, STDOUT_FILENO) == -1)
{
    perror("dup2 stdout");
    _exit(EXIT_FAILURE);
}

// Redirect standard error
if (dup2(slaveFd, STDERR_FILENO) == -1)
{
    perror("dup2 stderr");
    _exit(EXIT_FAILURE);
}

// The original slave descriptor is no longer needed.
close(slaveFd);

// Parent owns the master.
// Child doesn't need it.
close(masterFd);

 execl(
            "/home/kk/NovaShell/NovaShell",   // <-- Adjust if your CLI binary is elsewhere
            "NovaShell",
            (char *)nullptr
        );
        perror("execl");
        _exit(EXIT_FAILURE);
}
else{
    qDebug() << "Parent: Master FD =" << masterFd;
    qDebug() << "Parent: Slave PTY =" << slaveName;
    qDebug() << "Parent: Child PID =" << childPid;
}
notifier = new QSocketNotifier(masterFd,QSocketNotifier::Read,this);
connect(notifier,&QSocketNotifier::activated,this,&PtySession::readFromPty);

return true;
}

PtySession::PtySession(QObject *parent)
    : QObject(parent)
{
}

PtySession::~PtySession()
{
}
void PtySession::readFromPty()
{
    char buffer[4096];

    ssize_t bytes = read(masterFd, buffer, sizeof(buffer));

    if (bytes <= 0)
        return;
       
    emit dataReceived(QByteArray(buffer, bytes));
}
void PtySession::writeData(const QByteArray &data)
{   qDebug()<< "writting"<< data;
    if (masterFd != -1)
    {
        write(masterFd, data.constData(), data.size());
    }
}