#include "filesendtask.h"

FileSendTask::FileSendTask() : QRunnable()
{
    socket = new QTcpSocket();
    signaling = new Signaling();
}

void FileSendTask::setClientIp(QString ip)
{
    clientIp.setAddress(ip);
}

void FileSendTask::setClientPort(quint8 port)
{
    clientPort = port;
}

void FileSendTask::setFileList(QList<QString> list)
{
    fileList = list;
    fileNum = list.size();
}

void FileSendTask::run()
{
    foreach (QString filePath, fileList) {
       sendFile(filePath);
    }
}

void FileSendTask::sendFile(QString filePath)
{
    //发送时
    sem.acquire(1);

    //一个文件发送完毕
    quint64 nextFileSize;
    emit signaling->oneFileSendOver(nextFileSize);
}

void FileSendTask::updateSendProgress(quint64 numBytes)
{
    sem.release(1);
}

void FileSendTask::connectToClient()
{
    socket->connectToHost(clientIp,clientPort);
}
