#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    log = Log::getInstance();
    fileManagement = FileManagement::getInstance();
    settingDialog = new SettingDialog(this);

    initFileTreeView();
    readSettings();

    updateFileTreeView();
    connect(fileManagement,SIGNAL(workDirUpdated()),this,SLOT(updateFileTreeView()));
    connect(log,SIGNAL(logStr(QString)),this,SLOT(showLog(QString)));

    clientManagement  =new ClientManagement;
}

void MainWindow::showLog(QString l)
{
    ui->edtLog->append(l);
}

void MainWindow::initFileTreeView()
{
    fileModel = new QDirModel;
    fileModel->setReadOnly(false);
    fileModel->setSorting(QDir::DirsFirst|QDir::IgnoreCase|QDir::Name);
    ui->fileTreeView->setModel(fileModel);
    ui->fileTreeView->header()->setStretchLastSection(true);
    ui->fileTreeView->header()->setSortIndicator(0,Qt::AscendingOrder);
    ui->fileTreeView->header()->setSortIndicatorShown(true);

    ui->edtWorkDir->setText(fileManagement->getWorkDirectory());
}

MainWindow::~MainWindow()
{
    delete settingDialog;
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void MainWindow::writeSettings()
{
    QSettings settings("UESTC","LIMS FILE SERVER");
    settings.setValue("geometry",this->saveGeometry());
    settings.setValue("work directory",fileManagement->getWorkDirectory());
}

void MainWindow::readSettings()
{
    QSettings settings("UESTC","LIMS FILE SERVER");
    restoreGeometry(settings.value("geometry").toByteArray());
    fileManagement->setWorkDirectory(settings.value("work directory").toString());
}

void MainWindow::on_actionOptions_triggered()
{
    settingDialog->show();
}

void MainWindow::updateFileTreeView()
{
    QModelIndex index = fileModel->index(fileManagement->getWorkDirectory());
    ui->fileTreeView->expand(index);
    ui->fileTreeView->scrollTo(index);
    ui->fileTreeView->resizeColumnToContents(0);
    ui->edtWorkDir->setText(fileManagement->getWorkDirectory());
}


void MainWindow::on_btnTest_clicked()
{
    QUdpSocket socket;
    QJsonObject jo;
    jo.insert("SIGNALING_TYPE","PUSH_FILE");
    QJsonDocument jd;
    jd.setObject(jo);
    socket.writeDatagram(jd.toJson(),QHostAddress::LocalHost,SERVER_SIGNALING_PORT_UDP);
}
