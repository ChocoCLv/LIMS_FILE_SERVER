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
    connect(log,SIGNAL(logStr(quint8,QVariant)),this,SLOT(showLog(quint8,QVariant)));


    clientManagement  =new ClientManagement;

    connect(clientManagement,SIGNAL(getNewClient(int)),this,SLOT(getNewClient(int)));
    connect(clientManagement,SIGNAL(updateClientInfo(int)),this,SLOT(updateClientInfo(int)));

    initClientInfoTable();
}

void MainWindow::showLog(QString l)
{
    QTime time = QTime::currentTime();
    ui->edtLog->append(time.toString(Qt::TextDate)+"  "+l);
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

void MainWindow::initClientInfoTable()
{
    QStringList headerList;
    headerList<<"client ip"<<"receiving"<<"src client"<<"sending"<<"dst ip";
    clientModel.setHeaderList(headerList);
    clientModel.setClientMap(clientManagement->getClientMap());
    ui->tvClientTable->setModel(&clientModel);
}
void MainWindow::getNewClient(int index)
{
    clientModel.insertClient();
    clientModel.updateData(index);
}

void MainWindow::updateClientInfo(int index)
{
    clientModel.updateData(index);
}

void MainWindow::showLog(quint8 logType, QVariant logContent)
{
    switch(logType)
    {
    case Log::FILE_NAME_SEND:
        ui->edtFileName->setText(logContent.toString());
        break;
    case Log::FILE_SIZE_SEND:
        ui->progressBar->setMaximum(logContent.toInt());
        break;
    case Log::SEND_SIZE:
        ui->progressBar->setValue(logContent.toInt());
        break;
    case Log::DST_IP:
        ui->edtDstIP->setText(logContent.toString());
        break;
    case Log::COMMON_LOG:
        showLog(logContent.toString());
        break;
    default:
        showLog(logContent.toString());
        break;
    }
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
    QSettings *settings = new QSettings("config.ini",QSettings::IniFormat);
    settings->setValue("geometry",this->saveGeometry());
    settings->setValue("work directory",fileManagement->getWorkDirectory());
    delete settings;
}

void MainWindow::readSettings()
{
    QSettings *settings = new QSettings("config.ini",QSettings::IniFormat);
    restoreGeometry(settings->value("geometry").toByteArray());
    fileManagement->setWorkDirectory(settings->value("work directory").toString());
    delete settings;
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
    socket.writeDatagram(jd.toJson(),clientManagement->getLocalHostAddress(),SERVER_SIGNALING_PORT_UDP);
}
