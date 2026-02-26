// Fill out your copyright notice in the Description page of Project Settings.


#include "FBlueprintableMetaTag.h"


FBlueprintableMetaTag::FBlueprintableMetaTag(FName InTagName, EBPTagDataType InTagDataType)
	: TagName(InTagName), TagDataType(InTagDataType)
{
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

FBlueprintableMetaTag::~FBlueprintableMetaTag()
{
	GetRegisteredMetaTags().Remove(this);
}



TSet<const FBlueprintableMetaTag*>& FBlueprintableMetaTag::GetRegisteredMetaTags()
{
	static TSet<const class FBlueprintableMetaTag*> RegisteredNativeTags;
	return RegisteredNativeTags;
}