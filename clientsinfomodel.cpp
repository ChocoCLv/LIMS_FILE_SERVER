#include "clientsinfomodel.h"

ClientsInfoModel::ClientsInfoModel(QObject *parent) : QAbstractTableModel(parent)
{

}

int ClientsInfoModel::rowCount(const QModelIndex &parent) const
{
    return clientInfoMap->count();
}

void ClientsInfoModel::setHeaderList(QStringList &hl)
{
    headerList = hl;
}

void ClientsInfoModel::setClientMap(QMap<quint32, Client *> *cm)
{
    clientInfoMap  = cm;
}

int ClientsInfoModel::columnCount(const QModelIndex &parent) const
{
    return headerList.count();
}


QVariant ClientsInfoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole)
        return headerList.at(section);
    return QVariant();
}

QVariant ClientsInfoModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(role == Qt::TextAlignmentRole){
        return int(Qt::AlignRight | Qt::AlignCenter);
    }else if(role == Qt::DisplayRole){
        QString result;
        Client * client = clientInfoMap->values().at(index.row());
        switch(index.column())
        {
        case 0:
            result = client->getClientHostAddress().toString();
            break;
        case 1:
            result = client->getFileNameReceiving();
            break;
        case 2:
            result = client->getReceivingSrcIP();
            break;
        case 3:
            result = client->getFileNameSending();
            break;
        case 4:
            result = client->getSendingDstIP();
            break;
        default:
            result = "---";
            break;
        }
        return result;
    }
    return QVariant();
}

void ClientsInfoModel::updateData(int row)
{
    QModelIndex indexTL =createIndex(row,0);
    QModelIndex indexBR = createIndex(row,headerList.size());
    emit dataChanged(indexTL,indexBR);
}

void ClientsInfoModel::updateData(int row,int column)
{
    QModelIndex index =createIndex(row,column);
    emit dataChanged(index,index);
}

void ClientsInfoModel::insertClient()
{
    beginInsertRows(QModelIndex(),clientInfoMap->count(),clientInfoMap->count());
    insertRows(clientInfoMap->count(),1);
    endInsertRows();
}

ClientsInfoModel::~ClientsInfoModel()
{
    clientInfoMap = NULL;
}

