#include "settingdialog.h"
#include "ui_settingdialog.h"

SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    connect(ui->btnSelectWorkDir,SIGNAL(clicked(bool)),this,SLOT(selectWorkDir()));
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::selectWorkDir()
{
    dir = QFileDialog::getExistingDirectory(
                NULL,tr("选择工作目录"),"",QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    ui->edtWorkDir->setText(dir);
}

void SettingDialog::on_btnOK_clicked()
{
    FileManagement::getInstance()->setWorkDirectory(dir);
    this->close();
}


void SettingDialog::on_btnCancel_clicked()
{
    this->close();
}
