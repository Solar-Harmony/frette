using UnrealBuildTool;

public class FretteUI : ModuleRules
{
    public FretteUI(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CommonUI",
                "CoreUObject",
                "Engine",
                "Frette",
                "GameplayTags",
                "ModelViewViewModel",
                "Slate",
                "SlateCore", 
                "UMG"
            }
        );
    }
}