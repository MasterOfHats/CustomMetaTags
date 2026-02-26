// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


namespace UE::BPMetaTags::Private
{
	// Used to prevent people from putting BP_METATAG in their headers.
	constexpr bool IsFileCPP(const char* File)
	{
		const char* It = File;
		while (*It)
			++It;
		return It[-1] == 'p' && It[-2] == 'p' && It[-3] == 'c' && It[-4] == '.';
	}
}

#define BP_METATAG(Name, Type) static FBlueprintableMetaTag MetaTag##Name(#Name, Type); static_assert(UE::BPMetaTags::Private::IsFileCPP(__FILE__), "BP_METATAG and its derivatives can only be used in .cpp files");
#define BP_METATAG_BOOL(Name) BP_METATAG(Name, EBPTagDataType::Type_Bool);
#define BP_METATAG_STRING(Name) BP_METATAG(Name, EBPTagDataType::Type_String);
#define BP_METATAG_INT(Name) BP_METATAG(Name, EBPTagDataType::Type_Int);
#define BP_METATAG_FLOAT(Name) BP_METATAG(Name, EBPTagDataType::Type_Float);
#define BP_METATAG_LIST(Name, ...) static FBlueprintableMetaTag MetaTag##Name(#Name, EBPTagDataType::Type_List, {__VA_ARGS__}); static_assert(UE::BPMetaTags::Private::IsFileCPP(__FILE__), "BP_METATAG and its derivatives can only be used in .cpp files");

UENUM()
enum class EBPTagDataType
{
	Type_Bool,
	Type_String,
	Type_Int,
	Type_Float,
	Type_List
};
/**
 * 
 */
class CUSTOMMETATAGS_API FBlueprintableMetaTag : public FNoncopyable
{
public:
	FBlueprintableMetaTag(FName InTagName, EBPTagDataType InTagDataType);
	FBlueprintableMetaTag(FName InTagName, EBPTagDataType InTagDataType, TArray<FString> InAllowedElements);
	virtual ~FBlueprintableMetaTag();
	
	
	FName GetTagName() const;
	EBPTagDataType GetTagDataType() const;
	const TArray<TSharedPtr<FString>>& GetAllowedElements() const;

	static TSet<const FBlueprintableMetaTag*>& GetRegisteredMetaTags();
	
	private:
	FName TagName;
	EBPTagDataType TagDataType;
	
	TArray<TSharedPtr<FString>> AllowedElements;
};


