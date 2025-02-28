#include "Utilibrary.h"

#define LOCTEXT_NAMESPACE "FUtilibraryModule"

void FUtilibraryModule::StartupModule()
{	
}

void FUtilibraryModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUtilibraryModule, Utilibrary)

// 커스텀 로그
DEFINE_LOG_CATEGORY(UTL);
