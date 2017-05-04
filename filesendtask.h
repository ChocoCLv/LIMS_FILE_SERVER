#ifndef FILESENDTASK_H
#define FILESENDTASK_H

#include <QRunnable>
#include <QTcpSocket>
#include <QList>
#include <QString>
#include <QObject>
#include <QHostAddress>
#include <QSemaphore>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QDataStream>

#include "config.h"

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
    void setClientIp(QHostAddress ip);
    void setFileList(QList<QString> list);
    void connectToClient();
    void updateSendProgress(quint64 numBytes);

    //暴露给client
    //由于qrunnable不是qobject对象 不能使用connect
    QTcpSocket  *socket;
    Signaling   *signaling;

private:
    const static quint8 FILE_NAME = 1;
    const static quint8 FILE_DATA = 2;
    QHostAddress clientIp;
    QList<QString> fileList;
    QFile *currentSendFile;
    QByteArray fileBlock;
    QByteArray sndBlock;

    int fileNum;                        //文件总数量
    int fileDistributedNum;             //已经发送完成的文件数量
    quint64 currentFileSize;                //当前发送的文件总大小
    quint64 currentFileSizeDistributed;     //当前文件已经发送的大小
    quint64 numBytesSend;               //本次write成功的字节数
    void sendFile(QString filePath);    //发送某个文件
    void openFileRead(QString filePath);
    QSemaphore sem;

protected:
    void run();
};

#endif // FILESENDTASK_H
