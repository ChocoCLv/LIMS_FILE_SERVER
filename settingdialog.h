#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include "filemanagement.h"

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = 0);
    ~SettingDialog();

private:
    Ui::SettingDialog *ui;
    QString dir;

private slots:
    void selectWorkDir();
    void on_btnOK_clicked();
    void on_btnCancel_clicked();
};

#endif // SETTINGDIALOG_H
