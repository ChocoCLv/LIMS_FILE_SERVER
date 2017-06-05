#ifndef CLIENTMANAGEMENT_H
#define CLIENTMANAGEMENT_H

#include <QObject>
#include <QList>
#include <QJsonObject>
#include <QHostAddress>
#include <QQueue>
#include <QHostInfo>
#include <QMap>
#include <QList>

#include "client.h"
#include "signalingparsemodule.h"
#include "filemanagement.h"
#include "log.h"

class ClientManagement : public QObject
{
    Q_OBJECT
public:
    explicit ClientManagement(QObject *parent = 0);
    QHostAddress getLocalHostAddress();

    QMap<quint32, Client *>* getClientMap();
private:


    QMap<quint32,Client*> clientMap;

    SignalingParseModule *signalingParseModule;

    FileManagement *fileManagement;

    Log *log;

    void notifyTempServer(Client* server,Client* client,QString fileName);

    void allocServer();

    QHostAddress localHostAddr;
    Client* localClient;

    QMap<QString,QList<Client*>* > serverFileMap;
    QMap<QString,QList<Client*>* > clientFileMap;

    void initList();

    //记录初始状态下  主服务器发送到第几个文件  优先发送从未发送过的文件
    int currentSendFile;
    //是否存在未发送过的文件
    bool isExistUnSendFile;

signals:
    void getNewClient(int index);
    void updateClientInfo(int index);

public slots:
    void newClient(QHostAddress nc);
    void newTempServer(QHostAddress nc,QString fileName);
    void oneSendTaskOver();
    void releaseSendThread(QHostAddress addr);
};

#endif // CLIENTMANAGEMENT_H
