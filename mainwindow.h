#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QCloseEvent>
#include <QDebug>
#include <QDirModel>
#include <QDir>
#include <QModelIndex>
#include <QJsonObject>
#include <QJsonArray>
#include <QTime>
#include <QVariant>
#include <QStandardItemModel>

#include "filemanagement.h"
#include "settingdialog.h"
#include "clientmanagement.h"
#include "clientsinfomodel.h"
#include "config.h"
#include "log.h"

//测试用
#include <QUdpSocket>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void writeSettings();
    void readSettings();
    void initFileTreeView();
    void initClientInfoTable();
    FileManagement *fileManagement;
    SettingDialog *settingDialog;
    QDirModel *fileModel;
    ClientsInfoModel clientModel;
    ClientManagement *clientManagement;
    Log *log;


protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void updateFileTreeView();
    void on_actionOptions_triggered();
    void on_btnTest_clicked();
    void showLog(QString l);
    void showLog(quint8 logType,QVariant logContent);
    void getNewClient(int index);
    void updateClientInfo(int index);
};

#endif // MAINWINDOW_H
