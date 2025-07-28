#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

// 커스텀 로그
UTILIBRARY_API DECLARE_LOG_CATEGORY_EXTERN(UTL, Log, All);


class FUtilibraryModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

