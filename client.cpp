#include "client.h"

Client::Client(QObject *parent):QObject(parent)
{
    fileNum = 0;
    fileDistributedNum = 0;
    sendThreadUsed = 0;
    recvThreadUsed = 0;
}

void Client::acquireRecvThread()
{
    recvThreadUsed++;
}

void Client::acquireSendThread()
{
    sendThreadUsed++;
}

void Client::releaseRecvThread()
{
    recvThreadUsed--;
}

void Client::releaseSendThread()
{
    sendThreadUsed--;
}

bool Client::hasRestRecvThread()
{
    if(recvThreadUsed < MAX_RECV_THREAD){
        return true;
    }
    return false;

}

bool Client::hasRestSendThread()
{
    if(sendThreadUsed < MAX_SEND_THREAD){
        return true;
    }
    return false;
}

void Client::setWorkDir(QString dir)
{
    workDir = dir;
}

void Client::setClientIp(QHostAddress ip)
{
    clientIp = ip;
}

QHostAddress Client::getClientHostAddress()
{
    return clientIp;
}

void Client::pushFile(QHostAddress dst, QString fileName)
{
    FileSendTask* fileSendTask = new FileSendTask();
    fileSendTask->setClientIp(dst);
    fileSendTask->setFileName(fileName);
    fileSendTask->setWorkDir(workDir);
    QThread *sendThread = new QThread();
    fileSendTask->moveToThread(sendThread);
    connect(this,SIGNAL(startTask(QThread*)),fileSendTask,SLOT(startTask(QThread*)));
    connect(fileSendTask,SIGNAL(taskOver(FileSendTask*)),this,SLOT(releaseThreadResourse(FileSendTask*)));
    sendThread->start();
    emit startTask(sendThread);
}

void Client::releaseThreadResourse(FileSendTask *task)
{
    QThread *t = task->getThread();
    t->quit();
    task->deleteLater();
    t->deleteLater();
    emit taskOver();
}
