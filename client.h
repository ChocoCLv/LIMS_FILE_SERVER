#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QList>
#include <QHostAddress>


#include "filesendtask.h"

/**
 * @brief The ClientInfo class
 * 记录每个本次实验下载过文件的客户端信息
 * 管理自己的socket
 */

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = 0);
    void setClientIp(QHostAddress ip);
    void setWorkDir(QString dir);
    bool hasRestSendThread();
    bool hasRestRecvThread();
    void acquireSendThread();
    void acquireRecvThread();
    void releaseRecvThread();
    void releaseSendThread();
    void pushFile(QHostAddress dst,QString fileName);
    QHostAddress getClientHostAddress();

    QString getFileNameSending() const;
    void setFileNameSending(const QString &value);

    QString getSendingDstIP() const;
    void setSendingDstIP(const QString &value);

    QString getFileNameReceiving() const;
    void setFileNameReceiving(const QString &value);

    QString getReceivingSrcIP() const;
    void setReceivingSrcIP(const QString &value);

private:
    const static quint8 MAX_SEND_THREAD = 1;
    const static quint8 MAX_RECV_THREAD = 1;
    quint8 sendThreadUsed;
    quint8 recvThreadUsed;
    QString workDir;
    QHostAddress clientIp;          //客户端的IP

    QString fileNameSending;        //该客户端正在发送的文件名
    QString sendingDstIP;           //文件发送的目的客户端IP
    QString fileNameReceiving;      //该客户端正在接收的文件名
    QString receivingSrcIP;         //文件接收的源客户端IP

signals:
    void startTask(QThread*);
    void taskOver();

public slots:
    void releaseThreadResourse(FileSendTask* task);

};

#endif // CLIENTINFO_H
