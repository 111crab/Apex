// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Apex : ModuleRules
{
	public Apex(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"GameplayAbilities"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"GameplayTasks"
		});

		PublicDependencyModuleNames.Add("GameplayTags");

		PublicIncludePaths.AddRange(new string[] {
			"Apex",
			"Apex/Variant_Platforming",
			"Apex/Variant_Platforming/Animation",
			"Apex/Variant_Combat",
			"Apex/Variant_Combat/AI",
			"Apex/Variant_Combat/Animation",
			"Apex/Variant_Combat/Gameplay",
			"Apex/Variant_Combat/Interfaces",
			"Apex/Variant_Combat/UI",
			"Apex/Variant_SideScrolling",
			"Apex/Variant_SideScrolling/AI",
			"Apex/Variant_SideScrolling/Gameplay",
			"Apex/Variant_SideScrolling/Interfaces",
			"Apex/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
