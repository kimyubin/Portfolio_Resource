// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class kkumigi : ModuleRules
{
	public kkumigi(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"UMG",
			"OnlineSubsystem",
			"OnlineSubsystemSteam",
			"GameplayTags",
			"Utilibrary",
			
			"GeometryFramework",
			"GeometryCore",
			"DynamicMesh",
			"GeometryScriptingCore",
			"UnrealEd",
			
			"ProceduralMeshComponent",

			"MeshDescription",
			"RenderCore",
			"RHI",
			"StaticMeshDescription",
			"PhysicsCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

		AddEngineThirdPartyPrivateStaticDependencies(Target, "Steamworks");
	}
}