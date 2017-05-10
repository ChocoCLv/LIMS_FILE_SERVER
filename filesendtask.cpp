#include "filesendtask.h"

FileSendTask::FileSendTask(QObject *parent) : QObject(parent)
{
    fileNum = 0;
    fileDistributedNum = 0;
    currentFileSize = 0;
    currentFileSizeDistributed = 0;
}

void FileSendTask::setWorkDir(QString dir)
{
    workDir = dir+"/";
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

void FileSendTask::setTotalSize(quint64 size)
{
    totalSize = size;
}

void FileSendTask::startTask()
{
    sendTaskInfo();
}

void FileSendTask::updateSendProgress(qint64 numBytes)
{
    qDebug()<<QString("write bytes：%1").arg(numBytes);
    QString filePath;
    switch (lastDataType) {
    case TASK_INFO:
        filePath = fileList.at(fileDistributedNum);
        openFileRead(filePath);
        break;
    case FILE_NAME:
    case FILE_DATA:
        currentFileSizeDistributed += numBytes;
        sendFileData();
    default:
        break;
    }
}

void FileSendTask::sendFileData()
{
    if(currentFileSizeDistributed >= currentFileSize){
        fileDistributedNum++;
        startSendNewFile();
        return;
    }
    fileBlock = currentSendFile->read(1024);
    sndBlock.clear();
    QDataStream out(&sndBlock,QIODevice::WriteOnly);
    out<<quint16(0)<<FILE_DATA<<fileBlock;
    out.device()->seek(0);
    out<<quint16(sndBlock.size()-sizeof(quint16));

    socket->write(sndBlock);
    lastDataType = FILE_DATA;
    qDebug()<<QString("send file data：%1").arg(sndBlock.size());
}

void FileSendTask::startSendNewFile()
{
    if(fileDistributedNum == fileNum){
        emit taskCompleted();
        return;
    }
    QString filePath = fileList.at(fileDistributedNum);
    openFileRead(filePath);
}

void FileSendTask::connectToClient()
{
    qDebug()<<"send thread:"<<QThread::currentThreadId();
    socket = new QTcpSocket();
    connect(socket,SIGNAL(connected()),this,SLOT(startTask()));
    connect(socket,SIGNAL(bytesWritten(qint64)),this,SLOT(updateSendProgress(qint64)));
    socket->connectToHost(clientIp,FILE_PORT_TCP);
}

void FileSendTask::openFileRead(QString rFilePath)
{
    QString aFilePath = workDir +  rFilePath;
    currentSendFile = new QFile(aFilePath);
    QFileInfo fi = QFileInfo(aFilePath);

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

    socket->write(sndBlock);
    lastDataType = FILE_NAME;
}

void FileSendTask::sendTaskInfo()
{
    sndBlock.clear();
    QDataStream out(&sndBlock,QIODevice::WriteOnly);


    out<<quint16(0)<<TASK_INFO<<fileNum<<totalSize;
    out.device()->seek(0);
    out<<quint16(sndBlock.size()-sizeof(quint16));


    socket->write(sndBlock);
    lastDataType = TASK_INFO;
}


