// Fill out your copyright notice in the Description page of Project Settings.


#include "FBlueprintableMetaTag.h"



FBlueprintableMetaTag::FBlueprintableMetaTag(FName InTagName, FFieldClass* AllowedFields, EBPTagDataType InTagDataType)
	: TagName(InTagName), AllowedFields(AllowedFields), TagDataType(InTagDataType)
{
	GetRegisteredMetaTags().Add(this);
}

FBlueprintableMetaTag::FBlueprintableMetaTag(FName InTagName, FFieldClass* AllowedFields, EBPTagDataType InTagDataType,
	TArray<FString> InAllowedElements) : TagName(InTagName), AllowedFields(AllowedFields), TagDataType(InTagDataType)
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

FFieldClass* FBlueprintableMetaTag::GetAllowedFieldClass() const
{
	return AllowedFields;
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