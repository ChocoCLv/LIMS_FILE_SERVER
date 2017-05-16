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

private:
    const static quint8 MAX_SEND_THREAD = 1;
    const static quint8 MAX_RECV_THREAD = 1;
    quint8 sendThreadUsed;
    quint8 recvThreadUsed;
    QString workDir;
    QHostAddress clientIp;          //客户端的IP
    int fileNum;                    //文件总数量
    int fileDistributedNum;         //已经发送完成的文件数量

    QList<QString> currentFileList;
    void prepareDistribute();

signals:
    void startTask(QThread*);
    void taskOver();

public slots:
    void releaseThreadResourse(FileSendTask* task);

};

#endif // CLIENTINFO_H
