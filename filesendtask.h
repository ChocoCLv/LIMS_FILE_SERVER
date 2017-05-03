#ifndef FILESENDTASK_H
#define FILESENDTASK_H

#include <QRunnable>
#include <QTcpSocket>
#include <QList>
#include <QString>
#include <QObject>
#include <QHostAddress>
#include <QSemaphore>

class Signaling:public QObject
{
    Q_OBJECT
signals:
    oneFileSendOver(quint64 nextFileSize);
};

class FileSendTask : public QRunnable
{
public:
    FileSendTask();
    void setClientIp(QString ip);
    void setClientPort(quint8 port);
    void setFileList(QList<QString> list);
    void connectToClient();
    void updateSendProgress(quint64 numBytes);

    //暴露给client
    //由于qrunnable不是qobject对象 不能使用connect
    QTcpSocket *socket;
    Signaling *signaling;

private:
    QHostAddress clientIp;
    quint8 clientPort;
    QList<QString> fileList;

    int fileNum;                        //文件总数量
    int fileDistributedNum;             //已经发送完成的文件数量
    int currentFileSize;                //当前发送的文件总大小
    int currentFileSizeDistributed;     //当前文件已经发送的大小
    void sendFile(QString filePath);    //发送某个文件
    QSemaphore sem;

protected:
    void run();
};

#endif // FILESENDTASK_H
