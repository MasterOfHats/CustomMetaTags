#include "BlueprintVariableCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Engine/Blueprint.h"
#include "BlueprintEditorModule.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "FBlueprintableMetaTag.h"

#define LOCTEXT_NAMESPACE "BlueprintVariableMetaTagCustomization"

TSharedPtr<IDetailCustomization> FBlueprintVariableMetaTagCustomization::MakeInstance(
	TSharedPtr<IBlueprintEditor> InBlueprintEditor)
{
	return MakeShareable(new FBlueprintVariableMetaTagCustomization(InBlueprintEditor));
}

FBlueprintVariableMetaTagCustomization::FBlueprintVariableMetaTagCustomization(
	TSharedPtr<IBlueprintEditor> InBlueprintEditor)
	: BlueprintEditor(InBlueprintEditor)
{
	BlueprintWeakPtr = GetBlueprintFromEditor();
}

void FBlueprintVariableMetaTagCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
	DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);
	if (ObjectsBeingCustomized.Num() > 0 && BlueprintEditor.IsValid())
	{
		UPropertyWrapper* PropertyWrapper = Cast<UPropertyWrapper>(ObjectsBeingCustomized[0].Get());
		TWeakFieldPtr<FProperty> PropertyBeingCustomized = PropertyWrapper ? PropertyWrapper->GetProperty() : nullptr;

		auto FindPropPred = [PropertyBeingCustomized](FBPVariableDescription& Desc)
		{
			if (!PropertyBeingCustomized.IsValid()) return false;

			return PropertyBeingCustomized->GetFName() == Desc.VarName;
		};
		
		FBPVariableDescription* Found = BlueprintWeakPtr->NewVariables.FindByPredicate(FindPropPred);

		if(PropertyBeingCustomized.IsValid() && Found)
		{
			IDetailCategoryBuilder& MetadataCategory = DetailBuilder.EditCategory("Meta Tags");

			for (const FBlueprintableMetaTag* MetaTag : FBlueprintableMetaTag::GetRegisteredMetaTags())
			{
				FName TagName = MetaTag->GetTagName();
				FText TagDisplayName = FText::FromName(TagName);

				MetadataCategory.AddCustomRow(TagDisplayName)
					.NameContent()
					[
						SNew(STextBlock)
						.Text(TagDisplayName)
					]
					.ValueContent()
					[
						SNew(SCheckBox)
						.IsChecked_Lambda([PropertyBeingCustomized, TagName]() -> ECheckBoxState
						{
							if(!PropertyBeingCustomized.IsValid()) return ECheckBoxState::Unchecked;
							return PropertyBeingCustomized->HasMetaData(TagName) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
						})
						.OnCheckStateChanged_Lambda([PropertyBeingCustomized, FindPropPred, TagName, this](ECheckBoxState State)
						{
							if(!PropertyBeingCustomized.IsValid() || !BlueprintWeakPtr.IsValid()) return;

							FBPVariableDescription* FoundElement = BlueprintWeakPtr->NewVariables.FindByPredicate(FindPropPred);
							if(FoundElement)
							{
								if(FoundElement->HasMetaData(TagName))
								{
									FoundElement->RemoveMetaData(TagName);
								}
								else
								{
									FoundElement->SetMetaData(TagName, "");
								}
							}
							BlueprintWeakPtr->Modify();
							FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BlueprintWeakPtr.Get());
						})
					];

				MetadataCategory.AddCustomRow(TagDisplayName)
					.NameContent()
					[
						SNew(STextBlock)
						.Text(FText::Format(INVTEXT("{0} Value"), TagDisplayName))
					]
					.ValueContent()
					[
						SNew(SEditableTextBox)
						.Text_Lambda([PropertyBeingCustomized, TagName]() -> FText
						{
							if(!PropertyBeingCustomized.IsValid() || !PropertyBeingCustomized->HasMetaData(TagName))
								return FText::GetEmpty();
							return FText::FromString(PropertyBeingCustomized->GetMetaData(TagName));
						})
						.OnTextChanged_Lambda([FindPropPred, TagName, this](FText NewText)
						{
							if(!BlueprintWeakPtr.IsValid()) return;
							FBPVariableDescription* FoundElement = BlueprintWeakPtr->NewVariables.FindByPredicate(FindPropPred);
							if(FoundElement)
							{
								FoundElement->SetMetaData(TagName, NewText.ToString());
							}
							BlueprintWeakPtr->Modify();
							FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BlueprintWeakPtr.Get());
						})
					]
					.EditCondition(TAttribute<bool>::CreateLambda([PropertyBeingCustomized, TagName]() -> bool
					{
						if(!PropertyBeingCustomized.IsValid()) return false;
						return PropertyBeingCustomized->HasMetaData(TagName);
					}), FOnBooleanValueChanged());
			}
		}
	}
}

UBlueprint* FBlueprintVariableMetaTagCustomization::GetBlueprintFromEditor() const
{
	if (TSharedPtr<IBlueprintEditor> BPEd_AsShared = BlueprintEditor.Pin())
	{
		const TArray<UObject*>* Objects = (BlueprintEditor.IsValid() ? BPEd_AsShared->GetObjectsCurrentlyBeingEdited() : nullptr);
		if (Objects && Objects->Num() == 1)
		{
			if (UBlueprint* Blueprint = Cast<UBlueprint>((*Objects)[0]))
			{
				return Blueprint;
			}
		}
	}
	return nullptr;
}


#undef LOCTEXT_NAMESPACE
