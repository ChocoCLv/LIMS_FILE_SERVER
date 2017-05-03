#include "commmodule.h"

CommModule* CommModule::commModule = NULL;

CommModule::CommModule(QObject *parent) : QObject(parent)
{

}

CommModule* CommModule::getInstance()
{
    if(commModule == NULL){
        commModule = new CommModule;
    }
    return commModule;
}
