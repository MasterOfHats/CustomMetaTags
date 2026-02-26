// Fill out your copyright notice in the Description page of Project Settings.


#include "FBlueprintableMetaTag.h"

BP_METATAG_STRING(Custom1)
BP_METATAG_BOOL(Custom2)
BP_METATAG_FLOAT(Custom3)
BP_METATAG_INT(Custom4)

BP_METATAG_LIST(AllowedTags, "This", "That", "Other")

FBlueprintableMetaTag::FBlueprintableMetaTag(FName InTagName, EBPTagDataType InTagDataType)
	: TagName(InTagName), TagDataType(InTagDataType)
{
	GetRegisteredMetaTags().Add(this);
}

FBlueprintableMetaTag::FBlueprintableMetaTag(FName InTagName, EBPTagDataType InTagDataType,
	TArray<FString> InAllowedElements) : TagName(InTagName), TagDataType(InTagDataType)
{
	for (auto AllowedElement: InAllowedElements)
	{
		AllowedElements.Add(MakeShared<FString>(AllowedElement));
	}
	
	GetRegisteredMetaTags().Add(this);
}

FName FBlueprintableMetaTag::GetTagName() const
{
	return TagName;
}

EBPTagDataType FBlueprintableMetaTag::GetTagDataType() const
{
	return TagDataType;
}

const TArray<TSharedPtr<FString>>& FBlueprintableMetaTag::GetAllowedElements() const
{
	return AllowedElements;
}

FBlueprintableMetaTag::~FBlueprintableMetaTag()
{
	GetRegisteredMetaTags().Remove(this);
}



TSet<const FBlueprintableMetaTag*>& FBlueprintableMetaTag::GetRegisteredMetaTags()
{
	static TSet<const class FBlueprintableMetaTag*> RegisteredNativeTags;
	return RegisteredNativeTags;
}