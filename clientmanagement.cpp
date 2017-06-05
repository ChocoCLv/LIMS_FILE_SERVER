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
    clientMap[localHostAddr.toIPv4Address()] = localClient;
    connect(localClient,SIGNAL(taskOver()),this,SLOT(oneSendTaskOver()));
    connect(signalingParseModule,SIGNAL(releaseSendThread(QHostAddress)),
            this,SLOT(releaseSendThread(QHostAddress)));

    initList();
    currentSendFile = 0;
    isExistUnSendFile = true;
}

void ClientManagement::releaseSendThread(QHostAddress addr)
{
    Client *client = clientMap[addr.toIPv4Address()];
    client->setSendingDstIP("----");
    client->setFileNameSending("send complete!");
    int clientIndex = clientMap.keys().indexOf(addr.toIPv4Address());
    emit updateClientInfo(clientIndex);
    client->releaseSendThread();
    allocServer();
}

void ClientManagement::oneSendTaskOver()
{
    emit log->logStr("local client send over");
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
        serverList->append(localClient);
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
    clientMap[nc.toIPv4Address()] = client;

    emit log->logStr(QString("get client:%1").arg(nc.toString()));

    QMapIterator<QString,QList<Client*>* > i(clientFileMap);
    while(i.hasNext()){
        i.next();
        i.value()->append(client);
    }
    allocServer();
    emit getNewClient(clientMap.size()-1);
}

bool compareFileNum(const Client * c1, const Client * c2)
{
    if(c1->fileRecvedNum < c2->fileRecvedNum){
        return true;
    }
    return false;
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

    for(i = l.begin();i!=l.end();i++)
    {
        fileName = (*i);
        clientList = clientFileMap[fileName];
        serverList = serverFileMap[fileName];

        //按照文件数量排序
        qSort(serverList->begin(),serverList->end(),compareFileNum);

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
                notifyTempServer(server,client,fileName);
                clientList->removeAt(i);
                size--;
            }else{
                i++;
            }
        }
    }
}

void ClientManagement::notifyTempServer(Client* server, Client* client, QString fileName)
{
    QHostAddress serverIp = server->getClientHostAddress();
    QHostAddress clientIp = client->getClientHostAddress();
    server->setFileNameSending(fileName);
    server->setSendingDstIP(clientIp.toString());
    client->setFileNameReceiving(fileName);
    client->setReceivingSrcIP(serverIp.toString());
    int serverIndex = clientMap.keys().indexOf(serverIp.toIPv4Address());
    int clientIndex = clientMap.keys().indexOf(clientIp.toIPv4Address());
    emit updateClientInfo(clientIndex);
    emit updateClientInfo(serverIndex);

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
    Client *client = clientMap[nc.toIPv4Address()];
    serverFileMap[fileName]->append(client);
    client->fileRecvedNum++;
    emit log->logStr(QString("client:%1 recv file %2 over").arg(nc.toString()).arg(fileName));
    client->releaseRecvThread();
    client->setFileNameReceiving("recv complete!");
    client->setReceivingSrcIP("----");
    int clientIndex = clientMap.keys().indexOf(nc.toIPv4Address());
    emit updateClientInfo(clientIndex);
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

QMap<quint32, Client *> *ClientManagement::getClientMap()
{
    return &clientMap;
}
