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

#include "filemanagement.h"
#include "settingdialog.h"
#include "clientmanagement.h"
#include "config.h"

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
    FileManagement *fileManagement;
    SettingDialog *settingDialog;
    QDirModel *fileModel;
    ClientManagement *clientManagement;


protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void updateFileTreeView();
    void on_actionOptions_triggered();
    void on_btnTest_clicked();
};

#endif // MAINWINDOW_H
