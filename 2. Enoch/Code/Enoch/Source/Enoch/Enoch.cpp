// Copyright Epic Games, Inc. All Rights Reserved.

#include "Enoch.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(Enoch);

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, Enoch, "Enoch" );

static std::wstring path_enoch_data;

std::wstring GetEnochDataPath()
{
	if( path_enoch_data.empty())
	{
		// 릴리즈된 게임의 경로는 다를 수 있으니 확인
		FString cwd = FPaths::ProjectDir();
		cwd += "../../Data";
        path_enoch_data = TCHAR_TO_WCHAR(*cwd);
	}
	
	return std::wstring(path_enoch_data);
}
