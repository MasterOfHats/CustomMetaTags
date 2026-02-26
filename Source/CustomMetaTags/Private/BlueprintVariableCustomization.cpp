#include "BlueprintVariableCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SNumericEntryBox.h"
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
				EBPTagDataType TagDataType = MetaTag->GetTagDataType();

				// Bool tags: single checkbox row, no value editor
				if (TagDataType == EBPTagDataType::Type_Bool)
				{
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
							.OnCheckStateChanged_Lambda([FindPropPred, TagName, this](ECheckBoxState State)
							{
								if(!BlueprintWeakPtr.IsValid()) return;

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
					continue;
				}

				// Non-bool tags: enable checkbox row
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
						.OnCheckStateChanged_Lambda([FindPropPred, TagName, this](ECheckBoxState State)
						{
							if(!BlueprintWeakPtr.IsValid()) return;

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

				// Lambda to commit a value string to the meta data
				auto CommitValue = [FindPropPred, TagName, this](const FString& ValueStr)
				{
					if(!BlueprintWeakPtr.IsValid()) return;
					FBPVariableDescription* FoundElement = BlueprintWeakPtr->NewVariables.FindByPredicate(FindPropPred);
					if(FoundElement)
					{
						FoundElement->SetMetaData(TagName, ValueStr);
					}
					BlueprintWeakPtr->Modify();
					FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BlueprintWeakPtr.Get());
				};

				TSharedRef<SWidget> ValueWidget = SNullWidget::NullWidget;

				switch (TagDataType)
				{
				case EBPTagDataType::Type_String:
					ValueWidget = SNew(SEditableTextBox)
						.Text_Lambda([PropertyBeingCustomized, TagName]() -> FText
						{
							if(!PropertyBeingCustomized.IsValid() || !PropertyBeingCustomized->HasMetaData(TagName))
								return FText::GetEmpty();
							return FText::FromString(PropertyBeingCustomized->GetMetaData(TagName));
						})
						.OnTextChanged_Lambda([CommitValue](const FText& NewText)
						{
							CommitValue(NewText.ToString());
						});
					break;

				case EBPTagDataType::Type_Int:
					ValueWidget = SNew(SNumericEntryBox<int32>)
						.Value_Lambda([PropertyBeingCustomized, TagName]() -> TOptional<int32>
						{
							if(!PropertyBeingCustomized.IsValid() || !PropertyBeingCustomized->HasMetaData(TagName))
								return TOptional<int32>();
							return FCString::Atoi(*PropertyBeingCustomized->GetMetaData(TagName));
						})
						.OnValueChanged_Lambda([CommitValue](int32 NewValue)
						{
							CommitValue(FString::FromInt(NewValue));
						});
					break;

				case EBPTagDataType::Type_Float:
					ValueWidget = SNew(SNumericEntryBox<float>)
						.Value_Lambda([PropertyBeingCustomized, TagName]() -> TOptional<float>
						{
							if(!PropertyBeingCustomized.IsValid() || !PropertyBeingCustomized->HasMetaData(TagName))
								return TOptional<float>();
							return FCString::Atof(*PropertyBeingCustomized->GetMetaData(TagName));
						})
						.OnValueChanged_Lambda([CommitValue](float NewValue)
						{
							CommitValue(FString::SanitizeFloat(NewValue));
						});
					break;

				default:
					break;
				}

				MetadataCategory.AddCustomRow(TagDisplayName)
					.NameContent()
					[
						SNew(STextBlock)
						.Text(FText::Format(INVTEXT("{0} Value"), TagDisplayName))
					]
					.ValueContent()
					[
						ValueWidget
					]
					.Visibility(TAttribute<EVisibility>::CreateLambda([PropertyBeingCustomized, TagName]() -> EVisibility
					{
						if(!PropertyBeingCustomized.IsValid() || !PropertyBeingCustomized->HasMetaData(TagName))
							return EVisibility::Collapsed;
						return EVisibility::Visible;
					}));
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
