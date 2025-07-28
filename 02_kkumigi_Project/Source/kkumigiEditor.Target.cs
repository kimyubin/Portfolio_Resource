// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class kkumigiEditorTarget : TargetRules
{
	public kkumigiEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;     //V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest; //Unreal5_4;
		ExtraModuleNames.Add("kkumigi");
		RegisterModulesCreatedByRider();
	}

	private void RegisterModulesCreatedByRider()
	{
		ExtraModuleNames.AddRange(
			new string[]
			{
				"Utilibrary"
			}
		);
	}
}
