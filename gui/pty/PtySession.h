#pragma once

#include <QObject>

class QSocketNotifier;

class PtySession : public QObject
{
    Q_OBJECT

public:
    explicit PtySession(QObject *parent = nullptr);
    ~PtySession();

    bool start();
    void writeData(const QByteArray &data);

signals:
    void dataReceived(const QByteArray &data);
    void shellExited(int exitcode);

private slots:
    void readFromPty();

private:
    int masterFd{-1};
    pid_t childPid{-1};

    QSocketNotifier *notifier{nullptr};
};