#include "filesendtask.h"

FileSendTask::FileSendTask(QObject *parent) : QObject(parent)
{
    log = Log::getInstance();
    fileSize = 0;
    sndSize = 0;
    sndFile = new QFile;
}

void FileSendTask::setWorkDir(QString dir)
{
    workDir = dir;
}

void FileSendTask::setClientIp(QHostAddress ip)
{
    clientIp = ip;
}

void FileSendTask::setFileName(QString fn)
{
    fileName = fn;
}

void FileSendTask::startTask(QThread *t)
{
    emit log->logStr(Log::DST_IP,clientIp.toString());
    connectToClient();
    thread = t;
}

QThread * FileSendTask::getThread()
{
    return thread;
}

void FileSendTask::updateSendProgress(qint64 numBytes)
{
    sendFileData();
}

void FileSendTask::sendFileData()
{
    fileBlock = sndFile->read(SEND_BUFF_SIZE);
    if(fileBlock.size()==0){
        emit log->logStr(QString("file:%1 send complete").arg(sndFile->fileName()));
        sndFile->close();
        socket->disconnectFromHost();
        emit taskOver(this);
        return;
    }
    sndBlock.clear();
    QDataStream out(&sndBlock,QIODevice::WriteOnly);
    out<<quint16(0)<<FILE_DATA<<fileBlock;
    out.device()->seek(0);
    out<<quint16(sndBlock.size()-sizeof(quint16));
    sndSize += (sndBlock.size() - DATA_HEADER_SIZE);
    emit log->logStr(Log::SEND_SIZE,sndSize);

    socket->write(sndBlock);
}

void FileSendTask::connectToClient()
{
    socket = new QTcpSocket();
    connect(socket,SIGNAL(connected()),this,SLOT(openFileRead()));
    connect(socket,SIGNAL(bytesWritten(qint64)),this,SLOT(updateSendProgress(qint64)));
    socket->connectToHost(clientIp,FILE_PORT_TCP);
}

void FileSendTask::openFileRead()
{
    QString aFilePath = workDir +  fileName;
    sndFile->setFileName(aFilePath);
    QFileInfo fi = QFileInfo(aFilePath);

    if(!sndFile->open(QFile::ReadOnly)){
        emit log->logStr(QString("open %1,error:%2").arg(fileName).arg(sndFile->errorString()));
        return;
    }
    sndSize = 0;
    fileSize = fi.size();

    sndBlock.clear();
    QDataStream out(&sndBlock,QIODevice::WriteOnly);

    //发送文件与工作目录的相对文件路径
    out<<quint16(0)<<FILE_NAME<<fileSize<<fileName;
    out.device()->seek(0);
    out<<quint16(sndBlock.size()-sizeof(quint16));

    socket->write(sndBlock);
    emit log->logStr(Log::FILE_NAME_SEND,fileName);
    emit log->logStr(Log::FILE_SIZE_SEND,fileSize);
    emit log->logStr(QString("send file:%1 to client %2").
                     arg(sndFile->fileName()).arg(clientIp.toString()));
}

FileSendTask::~FileSendTask()
{
    sndFile->deleteLater();
    socket->close();
    socket->deleteLater();
}


