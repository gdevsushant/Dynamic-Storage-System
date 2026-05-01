#pragma once

#include "Modules/ModuleManager.h"
#include "Serialization/Custom/DSCustomSerializers_All.h"

class FDynamicStorageModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};