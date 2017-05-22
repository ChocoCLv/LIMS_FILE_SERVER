#ifndef FILESENDTASK_H
#define FILESENDTASK_H

#include <QTcpSocket>
#include <QString>
#include <QObject>
#include <QHostAddress>
#include <QFile>
#include <QFileInfo>
#include <QDataStream>
#include <QThread>

#include "config.h"
#include "log.h"

// 每个发送任务只发送一个文件

class FileSendTask : public QObject
{
    Q_OBJECT
public:
    FileSendTask(QObject *parent = 0);
    ~FileSendTask();
    void setClientIp(QHostAddress ip);
    void setWorkDir(QString dir);
    void setFileName(QString fn);
    void setThread(QThread* t);
    QThread * getThread();

private:
    QTcpSocket  *socket;
    const static quint8 FILE_NAME = 1;
    const static quint8 FILE_DATA = 2;

    /**
     * @brief 本次发送任务的信息
     */
    QString workDir;
    QHostAddress clientIp;

    QString fileName;
    QFile *sndFile;
    QByteArray fileBlock;
    QByteArray sndBlock;
    QThread *thread;

    quint64 fileSize;                //当前发送的文件总大小
    quint64 sndSize;     //当前文件已经发送的大小

    void sendFileData();

    /**
     * @brief isCurrentFileSendOver
     * 当上次发送的是文件数据 需要判断当前文件是否发送完成
     */

    Log* log;
    const static int DATA_HEADER_SIZE = sizeof(quint16) + sizeof(quint8) + 4;


signals:
    void taskOver(FileSendTask*);

public slots:
    void startTask(QThread* t);//启动发送任务
    void updateSendProgress(qint64 numBytes);
    void openFileRead();
    void connectToClient();
};

#endif // FILESENDTASK_H
