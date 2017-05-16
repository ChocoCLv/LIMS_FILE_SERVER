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

class ClientManagement : public QObject
{
    Q_OBJECT
public:
    explicit ClientManagement(QObject *parent = 0);

private:
    void getLocalHostAddress();

    QMap<quint32,Client*> clientList;

    SignalingParseModule *signalingParseModule;

    FileManagement *fileManagement;

    void notifyTempServer(QHostAddress serverIp,QHostAddress clientIp,QString fileName);

    void allocServer();

    QHostAddress localHostAddr;
    Client* localClient;

    QMap<QString,QList<Client*>* > serverFileMap;
    QMap<QString,QList<Client*>* > clientFileMap;

    void initList();

signals:

public slots:
    void newClient(QHostAddress nc);
    void newTempServer(QHostAddress nc,QString fileName);
    void oneSendTaskOver();
    void releaseSendThread(QHostAddress addr);
};

#endif // CLIENTMANAGEMENT_H
