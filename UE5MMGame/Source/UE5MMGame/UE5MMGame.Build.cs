// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UE5MMGame : ModuleRules
{
	public UE5MMGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "PakFile" });

        //PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}
