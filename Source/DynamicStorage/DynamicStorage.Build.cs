using System.IO;
using UnrealBuildTool;

public class DynamicStorage : ModuleRules
{
    public DynamicStorage(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "GameplayTags",
            "DeveloperSettings"
        });
        
        PublicIncludePaths.AddRange(new string[]
        {
            Path.Combine(ModuleDirectory, "Public/API")
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
        });
    }
}