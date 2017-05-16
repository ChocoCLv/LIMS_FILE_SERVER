#include "clientmanagement.h"

ClientManagement::ClientManagement(QObject *parent) : QObject(parent)
{
    signalingParseModule = new SignalingParseModule;
    getLocalHostAddress();
    fileManagement = FileManagement::getInstance();
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
}

void ClientManagement::initList()
{
    QList<QString> l = fileManagement->getCurrentFileList();
    QList<QString>::iterator i;
    for(i = l.begin();i!=l.end();i++)
    {
        QList<Client*>* serverList = new QList<Client*>;
        serverList->append(localClient);
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
    for(i = l.begin();i!=l.end();i++)
    {
        QString fileName = (*i);
        QList<Client*> *clientList = clientFileMap[fileName];
        QList<Client*> *serverList = serverFileMap[fileName];
        int size;
        if(clientList->size()<serverList->size()){
            size = clientList->size();
        }else{
            size = serverList->size();
        }
        for(int i = 0; i < size; )
        {
            Client* server = serverList->at(i);
            Client* client = clientList->at(i);
            if(server->hasRestSendThread()&&client->hasRestRecvThread()){
                server->acquireSendThread();
                client->acquireRecvThread();
                notifyTempServer(server->getClientHostAddress(),client->getClientHostAddress(),fileName);
                serverList->removeAt(i);
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
        localClient->pushFile(clientIp,fileName);
    }else{
        QJsonObject jo;
        jo.insert("SIGNALING_TYPE","PUSH_FILE_TO_CLIENT");
        jo.insert("CLIENT_IP",clientIp.toString());
        jo.insert("FILE_NAME",fileName);
        signalingParseModule->sendSignaling(jo,serverIp);
    }
}

void ClientManagement::newTempServer(QHostAddress nc, QString fileName)
{
    Client *client = clientList[nc.toIPv4Address()];
    serverFileMap[fileName]->append(client);
    client->releaseRecvThread();
    allocServer();
}

void ClientManagement::getLocalHostAddress()
{
    QString hostName = QHostInfo::localHostName();
    QHostInfo info = QHostInfo::fromName(hostName);
    foreach (QHostAddress address, info.addresses()) {
        if(address.protocol() == QAbstractSocket::IPv4Protocol){
            localHostAddr = address;
        }
    }
}
