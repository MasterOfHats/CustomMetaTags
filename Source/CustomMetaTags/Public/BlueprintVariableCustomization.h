// MIT License - Copyright (c) Louis Lamontagne

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
	static bool MatchesFieldClass(const FProperty* Property, FFieldClass* FieldClass);
	FBPVariableDescription* FindVariableDescription(FName VarName) const;
	
	
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
