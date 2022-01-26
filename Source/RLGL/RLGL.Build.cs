// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RLGL : ModuleRules
{
	public RLGL(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
