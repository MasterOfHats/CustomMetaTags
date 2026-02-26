// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#define BP_METATAG(Name, Type) inline FBlueprintableMetaTag MetaTag##Name(#Name, Type);
#define BP_METATAG_BOOL(Name) BP_METATAG(Name, EBPTagDataType::Type_Bool);
#define BP_METATAG_STRING(Name) BP_METATAG(Name, EBPTagDataType::Type_String);
#define BP_METATAG_INT(Name) BP_METATAG(Name, EBPTagDataType::Type_Int);
#define BP_METATAG_FLOAT(Name) BP_METATAG(Name, EBPTagDataType::Type_Float);

UENUM()
enum class EBPTagDataType
{
	Type_Bool,
	Type_String,
	Type_Int,
	Type_Float
};
/**
 * 
 */
class CUSTOMMETATAGS_API FBlueprintableMetaTag : public FNoncopyable
{
public:
	FBlueprintableMetaTag(FName InTagName, EBPTagDataType InTagDataType);
	virtual ~FBlueprintableMetaTag();
	
	
	FName GetTagName() const;
	EBPTagDataType GetTagDataType() const;

	static TSet<const FBlueprintableMetaTag*>& GetRegisteredMetaTags();
	
	private:
	FName TagName;
	EBPTagDataType TagDataType;
};


BP_METATAG_STRING(Custom1)
BP_METATAG_BOOL(Custom2)
BP_METATAG_FLOAT(Custom3)
BP_METATAG_INT(Custom4)