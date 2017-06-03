#ifndef LOG_H
#define LOG_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QTextStream>
#include <QFile>

class Log : public QObject
{
    Q_OBJECT
public:
    static Log* getInstance();
    static const quint8 COMMON_LOG = 5;
    static const quint8 FILE_SIZE_SEND = 6;
    static const quint8 FILE_NAME_SEND = 7;
    static const quint8 SEND_SIZE = 8;
    static const quint8 SEND_FILE_COMPLETE = 9;
    static const quint8 DST_IP = 10;

    void writeLog(QString l);

private:
    static Log* log;
    explicit Log(QObject *parent = 0);
    QFile *file;
    ~Log();

signals:
    void logStr(quint8 logType,QVariant logContent);
    void logStr(QString logContent);

public slots:
};

#endif // LOG_H
