#ifndef FILEMANAGEMENT_H
#define FILEMANAGEMENT_H

#include <QObject>
#include <QList>
#include <QDir>
#include <QFileInfoList>
#include <QTcpSocket>


/**
 * @brief The FileManagement class
 * 管理当前工作目录的文件
 * 记录当前实验所需的文件列表
 */

class FileManagement : public QObject
{
    Q_OBJECT
public:
    static FileManagement* getInstance();
    QString getWorkDirectory();
    void setWorkDirectory(QString dir);
    void setCurrentFileList(QList<QString> fileList);
    quint64 getTotalSize();
    int getFileNum();
    QList<QString> getCurrentFileList();

private:
    static FileManagement *fileManagement;
    explicit FileManagement(QObject *parent = 0);
    QString workDir;
    QList<QString> currentFileList;

    QList<QString> relativeFilePathList;
    QFileInfoList fileInfoList;
    QFileInfoList getFileList(QString path);
    QString projectName;
    quint64 totalSize;

    //由fileinfolist生成各文件与工作目录的相对文件路径
    void genFilePathList();
signals:
    void workDirUpdated();

public slots:
};

#endif // FILEMANAGEMENT_H
