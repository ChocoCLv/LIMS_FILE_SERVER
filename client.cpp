#include "client.h"

Client::Client(QObject *parent):QObject(parent)
{
    fileNum = 0;
    fileDistributedNum = 0;
    currentFileSize = 0;
    currentFileSizeDistributed = 0;

    isDistributeOver = false;
}

void Client::setWorkDir(QString dir)
{
    workDir = dir;
}

void Client::setClientIp(QHostAddress ip)
{
    clientIp = ip;
}

void Client::setServerIp(QHostAddress ip)
{
    serverIp = ip;
}

void Client::setIsDistributeOver(bool isOver)
{
    isDistributeOver = isOver;
}

void Client::setFileList(QList<QString> list)
{
    fileList = list;
}

QHostAddress Client::getClientHostAddress()
{
    return clientIp;
}

void Client::setTotalSize(quint64 size)
{
    totalSize = size;
}

void Client::prepareDistribute()
{
    fileSendTask = new FileSendTask();
    fileSendTask->setClientIp(clientIp);
    fileSendTask->setFileList(fileList);
    fileSendTask->setWorkDir(workDir);
    fileSendTask->setTotalSize(totalSize);
    QThread *sendThread = new QThread();
    fileSendTask->moveToThread(sendThread);
    connect(this,SIGNAL(startTask()),fileSendTask,SLOT(connectToClient()));
    sendThread->start();
    //fileSendTask->connectToClient();
    emit startTask();
    qDebug()<<"main thread:"<<QThread::currentThreadId();
}

void Client::updateSendProgress(qint64 numBytes)
{
    currentFileSizeDistributed += numBytes;
    fileSendTask->updateSendProgress(numBytes);
}

/**
 * @brief Client::oneFileDistributedOver
 * @param nextFileSize 下一个即将发送的文件的总大小
 * 发送完一个文件时 由filesendtask调用
 */
void Client::oneFileDistributedOver(quint64 nextFileSize)
{
    fileDistributedNum++;
    if(fileDistributedNum >= fileNum){
        isDistributeOver = true;
        return;
    }
    currentFileSize = nextFileSize;
    currentFileSizeDistributed = 0;
}


