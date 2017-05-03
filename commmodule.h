#ifndef COMMMODULE_H
#define COMMMODULE_H

#include <QObject>
#include <QTcpSocket>
#include <QUdpSocket>

/**
 * @brief The CommModule class
 * 负责数据收发
 * UDP发送控制信息
 * TCP发送文件
 */
class CommModule : public QObject
{
    Q_OBJECT
public:
    static CommModule* getInstance();

private:
    static CommModule* commModule;
    explicit CommModule(QObject *parent = 0);
    QTcpSocket tcpSocket;
    QUdpSocket udpSocket;

signals:

public slots:
};

#endif // COMMMODULE_H
