#ifndef CLIENTMANAGEMENT_H
#define CLIENTMANAGEMENT_H

#include <QObject>
#include <QList>
#include <QJsonObject>
#include <QHostAddress>
#include <QQueue>
#include <QHostInfo>
#include <QMap>

#include "client.h"
#include "signalingparsemodule.h"
#include "filemanagement.h"

class ClientManagement : public QObject
{
    Q_OBJECT
public:
    explicit ClientManagement(QObject *parent = 0);

private:
    QQueue<QHostAddress> tempServerIpQueue;
    QQueue<Client*> clientWaitForServerQueue;

    void getLocalHostAddress();

    QMap<QString,Client*> clientList;

    SignalingParseModule *signalingParseModule;

    FileManagement *fileManagement;

    void notifyTempServer(QHostAddress serverIp,QHostAddress clientIp);

    void allocServer(Client *client);

    QHostAddress localHostAddr;

signals:

public slots:
    void newClient(QHostAddress nc);
    void newTempServer(QHostAddress nc);
    void pushFile(QString clientIp);
};

#endif // CLIENTMANAGEMENT_H
