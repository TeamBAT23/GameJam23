// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GGJ23 : ModuleRules
{
	public GGJ23(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" });
	}
}
