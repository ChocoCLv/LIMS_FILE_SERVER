#include "signalingparsemodule.h"

SignalingParseModule::SignalingParseModule(QObject *parent) : QObject(parent)
{
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(SERVER_SIGNALING_PORT_UDP);
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(processPendingDatagrams()));

    clientSocket = new QUdpSocket(this);
    clientSocket->bind(CLIENT_SIGNALING_PORT_UDP);
    connect(clientSocket,SIGNAL(readyRead()),this,SLOT(processSignalingAsClient()));
}

SignalingParseModule::~SignalingParseModule()
{
    delete udpSocket;
}

void SignalingParseModule::processPendingDatagrams()
{
    QByteArray datagram;
    do{
        QHostAddress clientAddr;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(),datagram.size(),&clientAddr);
        QTextCodec *utf8codec = QTextCodec::codecForName("UTF-8");
        QString utf8str = utf8codec->toUnicode(datagram);
        datagram = utf8str.toStdString().data();
        processSignaling(datagram,clientAddr);
    }while(udpSocket->hasPendingDatagrams());
}

void SignalingParseModule::processSignalingAsClient()
{
    QByteArray datagram;
    do{
        QHostAddress clientAddr;
        datagram.resize(clientSocket->pendingDatagramSize());
        clientSocket->readDatagram(datagram.data(),datagram.size(),&clientAddr);
        QTextCodec *utf8codec = QTextCodec::codecForName("UTF-8");
        QString utf8str = utf8codec->toUnicode(datagram);
        datagram = utf8str.toStdString().data();
        QJsonParseError jpe;
        QJsonDocument jd = QJsonDocument::fromJson(datagram,&jpe);
        QJsonObject jo;

        if(jpe.error != QJsonParseError::NoError)
        {
            return;
        }
        jo = jd.object();

        QString signalingType = jo.find("SIGNALING_TYPE").value().toString();
        if(signalingType == "PUSH_FILE_TO_CLIENT"){
            QString clientIp = jo.find("CLIENT_IP").value().toString();
            emit pushFile(clientIp);
        }
    }while(clientSocket->hasPendingDatagrams());
}

void SignalingParseModule::processSignaling(QByteArray signaling, QHostAddress addr)
{
    QJsonParseError jpe;
    QJsonDocument jd = QJsonDocument::fromJson(signaling,&jpe);
    QJsonObject jo;

    if(jpe.error != QJsonParseError::NoError)
    {
        return;
    }
    jo = jd.object();

    QString signalingType = jo.find("SIGNALING_TYPE").value().toString();
    if(signalingType == "PUSH_FILE"){
        detectClient();
    }else if(signalingType == "I_AM_ALIVE"){
        emit clientFound(addr);
    }else if(signalingType == "RECV_OVER"){
        emit tempServerJoin(addr);
    }else if(signalingType == "PUSH_FILE_TO_CLIENT"){
        QString clientIp = jo.find("CLIENT_IP").value().toString();
        emit pushFile(clientIp);
    }
}

void SignalingParseModule::detectClient()
{
    QJsonObject jo;
    QJsonDocument jd;
    jo.insert("SIGNALING_TYPE","ARE_YOU_OK");
    jd.setObject(jo);
    QByteArray datagram;
    QDataStream out(&datagram,QIODevice::WriteOnly);
    out<<jd.toJson();
    udpSocket->writeDatagram(datagram,QHostAddress::Broadcast,CLIENT_SIGNALING_PORT_UDP);
    //udpSocket->writeDatagram(datagram,QHostAddress::LocalHost,CLIENT_SIGNALING_PORT_UDP);
}

void SignalingParseModule::sendSignaling(QJsonObject s, QHostAddress dst)
{
    QJsonDocument jd;
    jd.setObject(s);
    udpSocket->writeDatagram(jd.toJson(),dst,CLIENT_SIGNALING_PORT_UDP);
}
