// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class OmnibusTarget : TargetRules
{
	public OmnibusTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;     //V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest; //Unreal5_1;
		ExtraModuleNames.Add("Omnibus");
	}
}
