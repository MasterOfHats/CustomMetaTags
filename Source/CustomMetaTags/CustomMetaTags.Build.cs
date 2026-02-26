using UnrealBuildTool;

public class CustomMetaTags : ModuleRules
{
	public CustomMetaTags(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate",
			"SlateCore",
			"UnrealEd",
			"Kismet",
			"BlueprintGraph",
			"PropertyEditor",
			"EditorStyle",
			"InputCore",
		});
	}
}
