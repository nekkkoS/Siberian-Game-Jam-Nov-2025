// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectG_SGJNom2025 : ModuleRules
{
	public ProjectG_SGJNom2025(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] 
		{
			// Core
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput", 
			"PhysicsCore",
			"DeveloperSettings",
			// AI
			"AIModule", 
			"NavigationSystem",
			// UI
			"UMG", 
			"SlateCore",
			"Slate",
			// SFX
			"Niagara",
			// State Tree
			"GameplayStateTreeModule"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
