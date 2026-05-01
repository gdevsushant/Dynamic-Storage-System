// Copyright (c) Sushant 2026. All Rights Reserved.

using UnrealBuildTool;

public class DynamicStorageEditor : ModuleRules
{
    public DynamicStorageEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "DynamicStorage"   // 🔥 runtime dependency
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "UnrealEd",
            "BlueprintGraph",
            "KismetCompiler",
            "GraphEditor",
            "Slate",
            "SlateCore",
            "GameplayTags"
        });
    }
}