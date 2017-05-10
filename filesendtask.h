#ifndef FILESENDTASK_H
#define FILESENDTASK_H

#include <QTcpSocket>
#include <QList>
#include <QString>
#include <QObject>
#include <QHostAddress>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QDir>
#include <QDataStream>
#include <QThread>

#include "config.h"

class Signaling:public QObject
{
    Q_OBJECT
signals:

};

class FileSendTask : public QObject
{
    Q_OBJECT
public:
    FileSendTask(QObject *parent = 0);
    void setClientIp(QHostAddress ip);
    void setFileList(QList<QString> list);
    void setWorkDir(QString dir);


    void setTotalSize(quint64 size);


private:
    const static quint8 FILE_NAME = 1;
    const static quint8 FILE_DATA = 2;
    const static quint8 TASK_INFO = 3;
    QTcpSocket  *socket;

    /**
     * @brief 本次发送任务的信息
     */
    quint64 totalSize;
    QString workDir;
    QHostAddress clientIp;
    QList<QString> fileList;
    quint16 fileNum;


    QFile *currentSendFile;
    QByteArray fileBlock;
    QByteArray sndBlock;


    int fileDistributedNum;                 //已经发送完成的文件数量
    quint64 currentFileSize;                //当前发送的文件总大小
    quint64 currentFileSizeDistributed;     //当前文件已经发送的大小

    void sendFileData();
    void openFileRead(QString rFilePath);
    void sendTaskInfo();
    void startSendNewFile();

    /**
     * @brief lastDataType
     * 记录上次发送的数据类型，便于在收到发送成功的信号后确定下一步操作
     */
    quint8 lastDataType;

    /**
     * @brief isCurrentFileSendOver
     * 当上次发送的是文件数据 需要判断当前文件是否发送完成
     */
    bool isCurrentFileSendOver;

signals:
    void oneFileSendOver();
    void taskCompleted();
public slots:
    void startTask();//启动发送任务
    void updateSendProgress(qint64 numBytes);
    void connectToClient();
};

#endif // FILESENDTASK_H
