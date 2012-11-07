#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H

#include "Common.h"

namespace CCPlayer
{

class CCModuleManager
{
public:
    CCModuleManager();
    virtual ~CCModuleManager();

public:
    static void AddModule(MessageObjectId messageObjectId);
    static void DeleteModule(MessageObjectId messageObjectId);
};

}
#endif // MODULEMANAGER_H
