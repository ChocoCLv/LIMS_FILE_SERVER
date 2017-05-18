#include "clientmanagement.h"

ClientManagement::ClientManagement(QObject *parent) : QObject(parent)
{
    getLocalHostAddress();
    signalingParseModule = new SignalingParseModule;
    signalingParseModule->setLocalAddr(localHostAddr);
    fileManagement = FileManagement::getInstance();

    log = Log::getInstance();

    connect(signalingParseModule,SIGNAL(clientFound(QHostAddress)),
            this,SLOT(newClient(QHostAddress)));
    connect(signalingParseModule,SIGNAL(tempServerJoin(QHostAddress,QString)),
            this,SLOT(newTempServer(QHostAddress,QString)));

    localClient = new Client;
    localClient->setClientIp(localHostAddr);
    localClient->setWorkDir(fileManagement->getWorkDirectory());
    clientList[localHostAddr.toIPv4Address()] = localClient;
    connect(localClient,SIGNAL(taskOver()),this,SLOT(oneSendTaskOver()));
    connect(signalingParseModule,SIGNAL(releaseSendThread(QHostAddress)),
            this,SLOT(releaseSendThread(QHostAddress)));

    initList();
    currentSendFile = 0;
    isExistUnSendFile = true;
}

void ClientManagement::releaseSendThread(QHostAddress addr)
{
    Client *client = clientList[addr.toIPv4Address()];
    client->releaseSendThread();
    allocServer();
}

void ClientManagement::oneSendTaskOver()
{
    localClient->releaseSendThread();
    allocServer();
}

void ClientManagement::initList()
{
    QList<QString> l = fileManagement->getCurrentFileList();
    QList<QString>::iterator i;
    for(i = l.begin();i!=l.end();i++)
    {
        QList<Client*>* serverList = new QList<Client*>;
        //serverList->append(localClient);
        //在服务器未将所有文件发送完一遍时，不能将其加入服务器列表
        //避免服务器重复发送同一个文件
        serverFileMap[(*i)] = serverList;
        QList<Client*>* clientList = new QList<Client*>;
        clientFileMap[(*i)] = clientList;
    }
}

void ClientManagement::newClient(QHostAddress nc)
{
    Client *client = new Client;
    client->setClientIp(nc);
    clientList[nc.toIPv4Address()] = client;

    emit log->logStr(QString("get client:%1").arg(nc.toString()));

    QMapIterator<QString,QList<Client*>* > i(clientFileMap);
    while(i.hasNext()){
        i.next();
        i.value()->append(client);
    }
    allocServer();
}

void ClientManagement::allocServer()
{
    QList<QString> l = fileManagement->getCurrentFileList();
    QList<QString>::iterator i;
    QString fileName;
    QList<Client*> *clientList;
    QList<Client*> *serverList;
    Client* server;
    Client* client;

    if(currentSendFile < l.size()){
        emit log->logStr("还有未发送文件");
        fileName = l.at(currentSendFile);
        clientList = clientFileMap[fileName];
        if(clientList->at(0)->hasRestRecvThread()&&localClient->hasRestSendThread()){
            emit log->logStr("主服务器有可用发送线程");
            notifyTempServer(localClient->getClientHostAddress(),
                             clientList->at(0)->getClientHostAddress(),fileName);
            clientList->at(0)->acquireRecvThread();
            localClient->acquireSendThread();
            clientList->removeAt(0);
            currentSendFile++;
        }else{
            emit log->logStr("主服务器无可用发送线程");
        }
    }else{
        if(isExistUnSendFile){
            emit log->logStr("无未发送文件 主服务器进入服务器列表");
            //不存在未发送过的文件  主服务器与其他服务器地位相同  加入列表
            for(i = l.begin();i!=l.end();i++){
                serverFileMap[(*i)]->append(localClient);
            }
        }
        isExistUnSendFile = false;
    }


    for(i = l.begin();i!=l.end();i++)
    {
        fileName = (*i);
        clientList = clientFileMap[fileName];
        serverList = serverFileMap[fileName];
        int size;
        if(clientList->size()<serverList->size()){
            size = clientList->size();
        }else{
            size = serverList->size();
        }
        for(int i = 0; i < size; )
        {
            server = serverList->at(i);
            client = clientList->at(i);
            if(server->hasRestSendThread()&&client->hasRestRecvThread()){
                server->acquireSendThread();
                client->acquireRecvThread();
                emit log->logStr("常规分配服务器");
                notifyTempServer(server->getClientHostAddress(),client->getClientHostAddress(),fileName);
                clientList->removeAt(i);
                size--;
            }else{
                i++;
            }
        }
    }
}

void ClientManagement::notifyTempServer(QHostAddress serverIp, QHostAddress clientIp, QString fileName)
{

    if(serverIp.toIPv4Address() == localHostAddr.toIPv4Address()){
        emit log->logStr(QString("local server %1 send file:%2 to client %3").
                         arg(serverIp.toString()).arg(fileName).arg(clientIp.toString()));
        localClient->pushFile(clientIp,fileName);
    }else{
        QJsonObject jo;
        jo.insert("SIGNALING_TYPE","PUSH_FILE_TO_CLIENT");
        jo.insert("CLIENT_IP",clientIp.toString());
        jo.insert("FILE_NAME",fileName);
        signalingParseModule->sendSignaling(jo,serverIp);
        emit log->logStr(QString("server %1 send file:%2 to client %3").
                         arg(serverIp.toString()).arg(fileName).arg(clientIp.toString()));
    }
}

void ClientManagement::newTempServer(QHostAddress nc, QString fileName)
{
    Client *client = clientList[nc.toIPv4Address()];
    serverFileMap[fileName]->append(client);
    emit log->logStr(QString("client:%1 recv file %2 over").arg(nc.toString()).arg(fileName));
    client->releaseRecvThread();
    allocServer();
}

QHostAddress ClientManagement::getLocalHostAddress()
{
    QString hostName = QHostInfo::localHostName();
    QHostInfo info = QHostInfo::fromName(hostName);
    foreach (QHostAddress address, info.addresses()) {
        if(address.protocol() == QAbstractSocket::IPv4Protocol){
            localHostAddr = address;
            return address;
        }
    }
    return localHostAddr;
}
