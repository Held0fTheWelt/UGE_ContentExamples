using UnrealBuildTool;

public class UGE_ContentExamplesEditor : ModuleRules
{
	public UGE_ContentExamplesEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "UGE_ContentExamples" });
		PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd", "AssetRegistry", "BlueprintGraph" });
		PublicIncludePaths.Add(ModuleDirectory + "/../UGE_ContentExamples");
	}
}
