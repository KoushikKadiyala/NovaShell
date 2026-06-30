#include "PtySession.h"

#include <QCoreApplication>
#include <QDebug>
#include <QSocketNotifier>
#include <QString>

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/wait.h>

// #define WEXITSTATUS(status) __WEXITSTATUS (status)

bool PtySession::start(){
    masterFd = posix_openpt(O_RDWR | O_NOCTTY);
    fcntl(masterFd, F_SETFL, O_NONBLOCK);

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
QString shellPath =QCoreApplication::applicationDirPath()+"/NovaShell";
QByteArray path = shellPath.toLocal8Bit();

 execl(
            path.constData(),
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
    if (notifier)
        notifier->setEnabled(false);
    if(masterFd != -1)
    {
        close(masterFd);
        masterFd = -1;
    }
    if(childPid > -1)
    {
        kill(childPid,SIGTERM);
        waitpid(childPid, nullptr,0);
        childPid = -1;
    }
}
void PtySession::readFromPty()
{
    char buffer[4096];

    ssize_t bytes = read(masterFd, buffer, sizeof(buffer));

    if (bytes > 0)
    {
        emit dataReceived(QByteArray(buffer, bytes));
        return;
    }

    notifier->setEnabled(false);

    if (masterFd != -1)
    {
        close(masterFd);
        masterFd = -1;
    }

    int status = 0;          

    if (childPid > 0)
    {
        pid_t pid = waitpid(childPid, &status, 0);

        if (pid > 0)
        {
            childPid = -1;

            int exitCode = -1;

            if (WIFEXITED(status))
                exitCode = WEXITSTATUS(status);

            emit shellExited(exitCode);
        }
    }
}
void PtySession::writeData(const QByteArray &data)
{   qDebug()<< "writting"<< data;
    if (masterFd != -1)
    {
        write(masterFd, data.constData(), data.size());
    }
}