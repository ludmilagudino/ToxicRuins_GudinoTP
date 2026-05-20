// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ToxicRuins_GudinoTP : ModuleRules
{
	public ToxicRuins_GudinoTP(ReadOnlyTargetRules Target) : base(Target)
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
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"ToxicRuins_GudinoTP",
			"ToxicRuins_GudinoTP/Variant_Platforming",
			"ToxicRuins_GudinoTP/Variant_Platforming/Animation",
			"ToxicRuins_GudinoTP/Variant_Combat",
			"ToxicRuins_GudinoTP/Variant_Combat/AI",
			"ToxicRuins_GudinoTP/Variant_Combat/Animation",
			"ToxicRuins_GudinoTP/Variant_Combat/Gameplay",
			"ToxicRuins_GudinoTP/Variant_Combat/Interfaces",
			"ToxicRuins_GudinoTP/Variant_Combat/UI",
			"ToxicRuins_GudinoTP/Variant_SideScrolling",
			"ToxicRuins_GudinoTP/Variant_SideScrolling/AI",
			"ToxicRuins_GudinoTP/Variant_SideScrolling/Gameplay",
			"ToxicRuins_GudinoTP/Variant_SideScrolling/Interfaces",
			"ToxicRuins_GudinoTP/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
