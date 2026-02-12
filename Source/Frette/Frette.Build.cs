// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Frette : ModuleRules
{
	public Frette(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"NetCore"
		});

		PrivateDependencyModuleNames.AddRange([]);
	}
}