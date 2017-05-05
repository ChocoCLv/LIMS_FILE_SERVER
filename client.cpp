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
    connect(fileSendTask->socket,SIGNAL(connected()),this,SLOT(startDistribute()));
    connect(fileSendTask->socket,SIGNAL(bytesWritten(qint64)),this,SLOT(updateSendProgress(qint64)));
    connect(fileSendTask->signaling,SIGNAL(oneFileSendOver(quint64)),this,SLOT(oneFileDistributedOver(quint64)));
}

void Client::startDistribute()
{
    QThreadPool::globalInstance()->start(fileSendTask);
}

void Client::updateSendProgress(qint64 numBytes)
{
    currentFileSize += numBytes;
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


