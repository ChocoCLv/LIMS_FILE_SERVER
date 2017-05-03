#ifndef CLIENTMANAGEMENT_H
#define CLIENTMANAGEMENT_H

#include <QObject>
#include <QList>

#include "client.h"



class ClientManagement : public QObject
{
    Q_OBJECT
public:
    explicit ClientManagement(QObject *parent = 0);

private:
    QList<QString> tempServerIpList;

    QList<Client*> clientList;

signals:

public slots:
};

#endif // CLIENTMANAGEMENT_H
