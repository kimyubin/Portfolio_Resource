// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class OmnibusEditorTarget : TargetRules
{
	public OmnibusEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;     //V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest; //Unreal5_1;
		ExtraModuleNames.Add("Omnibus");
		RegisterModulesCreatedByRider();
	}

	private void RegisterModulesCreatedByRider()
	{
		ExtraModuleNames.AddRange(new string[] { "Utilibrary" });
	}
}
