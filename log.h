#ifndef LOG_H
#define LOG_H

#include <QObject>
#include <QString>

class Log : public QObject
{
    Q_OBJECT
public:
    static Log* getInstance();
    static const quint8 FILE_SIZE =1;
    static const quint8 FILE_NAME =2;
    static const quint8 SEND_SIZE =3;

private:
    explicit Log(QObject *parent = 0);
    static Log* log;
    ~Log();

signals:
    void logStr(QString );

public slots:
};

#endif // LOG_H
