#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QList>
#include <QThreadPool>
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
    void setServerIp(QHostAddress ip);
    void setIsDistributeOver(bool isOver);
    void setFileList(QList<QString> list);
    void prepareDistribute();
    QHostAddress getClientHostAddress();

private:
    FileSendTask *fileSendTask;
    QHostAddress clientIp;               //客户端的IP
    QHostAddress serverIp;               //主服务器为客户端分配的临时服务器的IP
    bool isDistributeOver;          //是否已经获取本次实验所需的资料
    QList<QString> fileList;
    int fileNum;                    //文件总数量
    int fileDistributedNum;         //已经发送完成的文件数量
    int currentFileSize;            //当前发送的文件总大小
    int currentFileSizeDistributed; //当前文件已经发送的大小

signals:

public slots:
    void startDistribute();
    void updateSendProgress(quint64 numBytes);
    void oneFileDistributedOver(quint64 nextFileSize);
};

#endif // CLIENTINFO_H
