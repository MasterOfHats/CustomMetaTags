// MIT License - Copyright (c) Louis Lamontagne

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"


/*
@Examples : Here are examples of implementation for the Blueprint Meta Tags. Note that these should be in a cpp file not a header. 

BP_METATAG_BOOL(TagDefault, FField)
BP_METATAG_STRING(TagStringData, FField)
BP_METATAG_FLOAT(TagFloatData, FField)
BP_METATAG_INT(TagIntData, FField)

BP_METATAG_LIST(TagDropdownList, FObjectProperty, "This", "That", "Other")
*/

class FCustomMetaTagsModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	FDelegateHandle VariableCustomizationHandle;
};
