// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DuoShooting : ModuleRules
{
	public DuoShooting(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG" });
	}
}
