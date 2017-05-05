#include "filemanagement.h"

FileManagement* FileManagement::fileManagement = NULL;

FileManagement::FileManagement(QObject *parent) : QObject(parent)
{
    totalSize = 0;
}

QList<QString> FileManagement::getCurrentFileList()
{
    return currentFileList;
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
    fileInfoList = getFileList(workDir);
    genFilePathList();

    //测试  即工作空间的所有文件均为项目所需文件
    setCurrentFileList(relativeFilePathList);

    emit workDirUpdated();
}

//存储的是与工作目录的相对路径
void FileManagement::setCurrentFileList(QList<QString> fileList)
{
    currentFileList = fileList;
    QList<QString>::iterator i;
    for(i = currentFileList.begin();i!=currentFileList.end();i++)
    {
        QString path = workDir + (*i);
        QFileInfo fi(path);
        totalSize += fi.size();
    }
}

QString FileManagement::getWorkDirectory()
{
    return workDir;
}

quint64 FileManagement::getTotalSize()
{
    return totalSize;
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

void FileManagement::genFilePathList()
{
    QDir dir(workDir);
    QFileInfoList::iterator i;
    i = fileInfoList.begin();
    for(;i!=fileInfoList.end();i++){
        QString absoluteFilePath = (*i).absoluteFilePath();
        QString relativeFilePath = dir.relativeFilePath(absoluteFilePath);
        totalSize += (*i).size();
        relativeFilePathList.append(relativeFilePath);
    }
}
