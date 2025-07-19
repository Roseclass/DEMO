// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DEMO : ModuleRules
{
	public DEMO(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"Slate",
			"SlateCore",
			"HeadMountedDisplay",
			"NavigationSystem",
			"AIModule",
			"UMG",
			"ProceduralMeshComponent",
			"Niagara",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
		});

		PublicIncludePaths.Add(ModuleDirectory);
	}
}
