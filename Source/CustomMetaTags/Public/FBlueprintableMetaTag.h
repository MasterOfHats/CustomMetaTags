// MIT License - Copyright (c) Louis Lamontagne

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

#define BP_METATAG(Name, FieldType, Type) static FBlueprintableMetaTag MetaTag##Name(#Name, FieldType::StaticClass(), Type); static_assert(UE::BPMetaTags::Private::IsFileCPP(__FILE__), "BP_METATAG and its derivatives can only be used in .cpp files");

#define BP_METATAG_BOOL(Name, FieldType) BP_METATAG(Name, FieldType, EBPTagDataType::Type_Bool);
#define BP_METATAG_STRING(Name, FieldType) BP_METATAG(Name, FieldType, EBPTagDataType::Type_String);
#define BP_METATAG_INT(Name, FieldType) BP_METATAG(Name, FieldType, EBPTagDataType::Type_Int);
#define BP_METATAG_FLOAT(Name, FieldType) BP_METATAG(Name, FieldType, EBPTagDataType::Type_Float);
#define BP_METATAG_LIST(Name, FieldType, ...) static FBlueprintableMetaTag MetaTag##Name(#Name, FieldType::StaticClass(), EBPTagDataType::Type_List, {__VA_ARGS__}); static_assert(UE::BPMetaTags::Private::IsFileCPP(__FILE__), "BP_METATAG and its derivatives can only be used in .cpp files");



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
class FBlueprintableMetaTag : public FNoncopyable
{
public:
	CUSTOMMETATAGS_API FBlueprintableMetaTag(FName InTagName, FFieldClass* AllowedFields, EBPTagDataType InTagDataType);
	CUSTOMMETATAGS_API FBlueprintableMetaTag(FName InTagName, FFieldClass* AllowedFields, EBPTagDataType InTagDataType, TArray<FString> InAllowedElements);
	CUSTOMMETATAGS_API virtual ~FBlueprintableMetaTag();
	
	
	FName GetTagName() const;
	EBPTagDataType GetTagDataType() const;
	FFieldClass* GetAllowedFieldClass() const;
	const TArray<TSharedPtr<FString>>& GetAllowedElements() const;

	static TSet<const FBlueprintableMetaTag*>& GetRegisteredMetaTags();
	
	private:
	FName TagName;
	FFieldClass* AllowedFields;
	EBPTagDataType TagDataType;
	
	TArray<TSharedPtr<FString>> AllowedElements;
	

};


