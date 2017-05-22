#ifndef CLIENTSINFOMODEL_H
#define CLIENTSINFOMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QMap>
#include "client.h"

class ClientsInfoModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ClientsInfoModel(QObject *parent = 0);
    void setClientMap(QMap<quint32, Client *> *cm);
    void setHeaderList(QStringList &hl);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void updateData(int row);
    void updateData(int row,int column);
    void insertClient();
    ~ClientsInfoModel();

private:
    QMap<quint32, Client*> *clientInfoMap;
    QStringList headerList;

signals:

public slots:
};

#endif // CLIENTSINFOMODEL_H
