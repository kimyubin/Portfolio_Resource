// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class kkumigiTarget : TargetRules
{
	public kkumigiTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
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
