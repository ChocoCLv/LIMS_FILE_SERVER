#include "log.h"

Log* Log::log = NULL;

Log::Log(QObject *parent) : QObject(parent)
{

}

Log* Log::getInstance()
{
    if(log == NULL){
        log = new Log;
    }
    return log;
}

Log::~Log()
{
    delete log;
}
