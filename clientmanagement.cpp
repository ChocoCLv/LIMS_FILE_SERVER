#include "clientmanagement.h"

ClientManagement::ClientManagement(QObject *parent) : QObject(parent)
{
    signalingParseModule = new SignalingParseModule;
    getLocalHostAddress();
    fileManagement = FileManagement::getInstance();
    connect(signalingParseModule,SIGNAL(clientFound(QHostAddress)),this,SLOT(newClient(QHostAddress)));
    connect(signalingParseModule,SIGNAL(tempServerJoin(QHostAddress)),this,SLOT(newTempServer(QHostAddress)));
    connect(signalingParseModule,SIGNAL(pushFile(QString)),this,SLOT(pushFile(QString)));
}

void ClientManagement::newClient(QHostAddress nc)
{
    Client *client = new Client;
    client->setClientIp(nc);
    clientList.insert(nc.toString(),client);
    allocServer(client);
}

void ClientManagement::allocServer(Client *client)
{
    if(tempServerIpQueue.size() != 0){
        QHostAddress serverIp = tempServerIpQueue.dequeue();
        client->setServerIp(serverIp);
        notifyTempServer(serverIp,client->getClientHostAddress());
    }else{
        clientWaitForServerQueue.enqueue(client);
    }
}

void ClientManagement::notifyTempServer(QHostAddress serverIp, QHostAddress clientIp)
{
    QJsonObject jo;
    jo.insert("SIGNALING_TYPE","PUSH_FILE_TO_CLIENT");
    jo.insert("CLIENT_IP",clientIp.toString());
    signalingParseModule->sendSignaling(jo,serverIp);
}

void ClientManagement::pushFile(QString clientIp)
{
    Client *client = new Client;
    QHostAddress clientAddr;
    clientAddr.setAddress(clientIp);
    client->setClientIp(clientAddr);
    client->setFileList(fileManagement->getCurrentFileList());
    client->setWorkDir(fileManagement->getWorkDirectory());
    client->prepareDistribute();
}

void ClientManagement::newTempServer(QHostAddress nc)
{
    tempServerIpQueue.enqueue(nc);
    if(clientWaitForServerQueue.size() != 0){
        allocServer(clientWaitForServerQueue.dequeue());
    }
}

void ClientManagement::getLocalHostAddress()
{
    QString hostName = QHostInfo::localHostName();
    QHostInfo info = QHostInfo::fromName(hostName);
    foreach (QHostAddress address, info.addresses()) {
        if(address.protocol() == QAbstractSocket::IPv4Protocol){
            localHostAddr = address;
            tempServerIpQueue.enqueue(address);
        }
    }
}
