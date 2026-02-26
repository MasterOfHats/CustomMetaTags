#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class UBlueprint;

class FBlueprintVariableMetaTagCustomization : public IDetailCustomization
{
public:
	static TSharedPtr<IDetailCustomization> MakeInstance(TSharedPtr<IBlueprintEditor> InBlueprintEditor);

	FBlueprintVariableMetaTagCustomization(TSharedPtr<IBlueprintEditor> InBlueprintEditor);

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
;

private:
	UBlueprint* GetBlueprintFromEditor() const;
	
	
	TWeakPtr<IBlueprintEditor> BlueprintEditor;
	TWeakObjectPtr<UBlueprint> BlueprintWeakPtr;
	IDetailLayoutBuilder* CachedDetailLayout = nullptr;

	struct FMetaTagEntry
	{
		FString Key;
		FString Value;
	};

	TArray<FMetaTagEntry> MetaTags;
};
