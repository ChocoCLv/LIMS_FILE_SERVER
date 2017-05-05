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
    ~SignalingParseModule();

private:
    QUdpSocket *udpSocket;
    QUdpSocket *clientSocket;//只处理推送文件到客户端的命令

    void processSignaling(QByteArray signaling,QHostAddress addr);

    void detectClient();

signals:
    void clientFound(QHostAddress addr);
    void tempServerJoin(QHostAddress addr);
    void pushFile(QString clientIp);

public slots:
    void processPendingDatagrams();
    void processSignalingAsClient();
};

#endif // SIGNALINGPARSEMODULE_H
