#ifndef SIGNALINGPARSEMODULE_H
#define SIGNALINGPARSEMODULE_H

#include <QObject>
#include <QUdpSocket>
#include <QJsonObject>
#include <QJsonParseError>
#include <QHostAddress>
#include <QTextCodec>
#include <QDataStream>

#include "config.h"
#include "log.h"
#include "filemanagement.h"

/**
 * @brief The SignalingParseModule class
 * 负责处理请求
 */

class SignalingParseModule : public QObject
{
    Q_OBJECT
public:
    explicit SignalingParseModule(QObject *parent = 0);
    void sendSignaling(QJsonObject s,QHostAddress dst);
    void setLocalAddr(QHostAddress local);
    ~SignalingParseModule();

private:
    QUdpSocket *udpSocket;
    QHostAddress localAddr;

    void processSignaling(QByteArray signaling,QHostAddress addr);

    void detectClient();

    Log *log;

signals:
    void clientFound(QHostAddress addr);
    void tempServerJoin(QHostAddress addr,QString fileName);
    void pushFile(QString clientIp,QString fileName);
    void releaseSendThread(QHostAddress addr);

public slots:
    void processPendingDatagrams();
};

#endif // SIGNALINGPARSEMODULE_H
