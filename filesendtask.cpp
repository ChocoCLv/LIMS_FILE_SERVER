#include "filesendtask.h"

FileSendTask::FileSendTask() : QRunnable()
{
    socket = new QTcpSocket();
    signaling = new Signaling();
    sem.release(1);

    fileNum = 0;
    fileDistributedNum = 0;
    currentFileSize = 0;
    currentFileSizeDistributed = 0;
    numBytesSend = 0;
}

void FileSendTask::setWorkDir(QString dir)
{
    workDir = dir+QDir::separator();
}

void FileSendTask::setClientIp(QHostAddress ip)
{
    clientIp = ip;
}

void FileSendTask::setFileList(QList<QString> list)
{
    fileList = list;
    fileNum = list.size();
}

void FileSendTask::run()
{
    sem.release(1);
    sendReady();
    foreach (QString filePath, fileList) {
        //一个文件发送完毕
        sendFile(filePath);
        delete currentSendFile;
    }
}

void FileSendTask::setTotalSize(quint64 size)
{
    totalSize = size;
}

void FileSendTask::sendFile(QString filePath)
{
    openFileRead(filePath);
    //发送时
    while(1){
        currentFileSizeDistributed += numBytesSend;
        if(currentFileSizeDistributed >= currentFileSize){
            return;
        }
        fileBlock = currentSendFile->read(1024);
        sndBlock.clear();
        QDataStream out(&sndBlock,QIODevice::WriteOnly);
        out<<quint16(0)<<FILE_DATA<<fileBlock;
        out.device()->seek(0);
        out<<quint16(sndBlock.size()-sizeof(quint16));

        //等待成功发送的信号
        sem.acquire(1);
        socket->write(sndBlock);
    }
}

void FileSendTask::updateSendProgress(quint64 numBytes)
{
    numBytesSend = numBytes;
    sem.release(1);
}

void FileSendTask::connectToClient()
{
    socket->connectToHost(clientIp,FILE_PORT_TCP);
}

void FileSendTask::openFileRead(QString rFilePath)
{
    QString aFilePath = workDir +  rFilePath;
    currentSendFile = new QFile(aFilePath);
    QFileInfo fi = QFileInfo(aFilePath);
    quint64 nextFileSize = fi.size();
    emit signaling->oneFileSendOver(nextFileSize);
    if(!currentSendFile->open(QFile::ReadOnly)){
        qDebug()<<"open file error-read"<<endl;
        return;
    }
    currentFileSizeDistributed = 0;
    currentFileSize = fi.size();

    sndBlock.clear();
    QDataStream out(&sndBlock,QIODevice::WriteOnly);

    //发送文件与工作目录的相对文件路径
    out<<quint16(0)<<FILE_NAME<<currentFileSize<<rFilePath;
    out.device()->seek(0);
    out<<quint16(sndBlock.size()-sizeof(quint16));

    sem.acquire(1);
    socket->write(sndBlock);
}


void FileSendTask::sendReady()
{
    sndBlock.clear();
    QDataStream out(&sndBlock,QIODevice::WriteOnly);

    //发送文件与工作目录的相对文件路径
    out<<quint16(0)<<TASK_INFO<<fileNum<<totalSize;
    out.device()->seek(0);
    out<<quint16(sndBlock.size()-sizeof(quint16));

    sem.acquire(1);
    socket->write(sndBlock);
}


