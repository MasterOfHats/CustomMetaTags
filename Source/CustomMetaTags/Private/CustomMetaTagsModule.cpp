// MIT License - Copyright (c) Louis Lamontagne

#include "CustomMetaTagsModule.h"
#include "BlueprintVariableCustomization.h"
#include "BlueprintEditorModule.h"

#define LOCTEXT_NAMESPACE "FCustomMetaTagsModule"

void FCustomMetaTagsModule::StartupModule()
{
	FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");

	VariableCustomizationHandle = BlueprintEditorModule.RegisterVariableCustomization(
		FProperty::StaticClass(),
		FOnGetVariableCustomizationInstance::CreateStatic(&FBlueprintVariableMetaTagCustomization::MakeInstance)
	);
}

void FCustomMetaTagsModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("Kismet"))
	{
		FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::GetModuleChecked<FBlueprintEditorModule>("Kismet");
		BlueprintEditorModule.UnregisterVariableCustomization(FProperty::StaticClass(), VariableCustomizationHandle);
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCustomMetaTagsModule, CustomMetaTags)
