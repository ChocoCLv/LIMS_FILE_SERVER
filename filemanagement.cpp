#include "filemanagement.h"

FileManagement* FileManagement::fileManagement = NULL;

FileManagement::FileManagement(QObject *parent) : QObject(parent)
{

}

FileManagement* FileManagement::getInstance()
{
    if(fileManagement == NULL){
        fileManagement = new FileManagement();
    }
    return fileManagement;
}

void FileManagement::setWorkDirectory(QString dir)
{
    workDir = dir;
    fileList = getFileList(workDir);
    emit workDirUpdated();
}

QString FileManagement::getWorkDirectory()
{
    return workDir;
}

QFileInfoList FileManagement::getFileList(QString path)
{
    QDir dir(path);
    QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for(int i = 0; i != folder_list.size(); i++)
    {
         QString name = folder_list.at(i).absoluteFilePath();
         QFileInfoList child_file_list = getFileList(name);
         file_list.append(child_file_list);
    }

    return file_list;
}
