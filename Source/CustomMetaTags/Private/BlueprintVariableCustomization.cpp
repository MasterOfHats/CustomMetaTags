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

#define LOCTEXT_NAMESPACE "BlueprintVariableMetaTagCustomization"

static const FName ContentFolderMeta(TEXT("BasicMetaTag"));

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
			
			//Metadata stuff
			IDetailCategoryBuilder& MetadataCategory = DetailBuilder.EditCategory("Something");
			
			MetadataCategory.AddCustomRow(INVTEXT("ContentFilter"))
				.NameContent()
				[
					SNew(STextBlock)
					.Text(INVTEXT("Content Filter"))
				]
				.ValueContent()
				[
					SNew(SCheckBox)
					.IsChecked_Lambda([PropertyBeingCustomized]() -> ECheckBoxState
					{
						if(!PropertyBeingCustomized.IsValid()) return ECheckBoxState::Unchecked;
						
						return PropertyBeingCustomized->HasMetaData(ContentFolderMeta) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
					})
					.OnCheckStateChanged_Lambda([PropertyBeingCustomized, FindPropPred, this](ECheckBoxState State)
					{
						if(!PropertyBeingCustomized.IsValid()) return;
						
						if(BlueprintWeakPtr.IsValid())
						{
							FBPVariableDescription* FoundElement = BlueprintWeakPtr->NewVariables.FindByPredicate(FindPropPred);

							if(FoundElement)
							{
								if(FoundElement->HasMetaData(ContentFolderMeta))
								{
									FoundElement->RemoveMetaData(ContentFolderMeta);
								} else
								{
									FoundElement->SetMetaData(ContentFolderMeta, "");
								}
								
							}
							BlueprintWeakPtr->Modify();
							FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BlueprintWeakPtr.Get());
						}
					})
				];

			MetadataCategory.AddCustomRow(INVTEXT("Content Filter Area"))
			.NameContent()
				[
					SNew(STextBlock)
						.Text(INVTEXT("Content Filter Path"))
				]
			.ValueContent()
			[
				SNew(SEditableTextBox)
				.Text_Lambda([PropertyBeingCustomized]() -> FText
				{
					return FText::FromString( PropertyBeingCustomized->HasMetaData(ContentFolderMeta) ? FName(PropertyBeingCustomized->GetMetaData(ContentFolderMeta)).ToString() : "");
				})
				.OnTextChanged_Lambda([FindPropPred, this](FText NewText)
				{
					FBPVariableDescription* FoundElement = BlueprintWeakPtr->NewVariables.FindByPredicate(FindPropPred);
					if(FoundElement)
					{
						FoundElement->SetMetaData(ContentFolderMeta, NewText.ToString());
					}

					BlueprintWeakPtr->Modify();
					FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BlueprintWeakPtr.Get());
				})
			]
			.EditCondition(TAttribute<bool>::CreateLambda([PropertyBeingCustomized]() -> bool
			{
				if(!PropertyBeingCustomized.IsValid()) return false;
				return PropertyBeingCustomized->HasMetaData(ContentFolderMeta);
			}), FOnBooleanValueChanged());


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
