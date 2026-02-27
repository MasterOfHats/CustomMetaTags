# CustomMetaTags Plugin

An Unreal Engine editor plugin that lets you define custom metadata tags for Blueprint variables, with automatic detail panel UI generation.

FProperties can carry metadata in key-value string pairs. 
For C++ reflected properties, adding meta tags is trivial. However for blueprints you usually to manually extend the property details each time 

This plugin provides:


1. **A registration system** for declaring custom meta tags with typed values
2. **Automatic editor UI** that appears in the Blueprint variable details panel
3. **Field class filtering** so tags only show up on relevant property types

## Quick Start

### 1. Add the dependency

In your module's `.Build.cs`:

```csharp
PrivateDependencyModuleNames.Add("CustomMetaTags");
```

### 2. Define meta tags

In any `.cpp` file:

```cpp
#include "FBlueprintableMetaTag.h"

BP_METATAG_BOOL(MyFlag, FField)                           // Checkbox for all properties
BP_METATAG_STRING(Description, FField)                     // Text field for all properties
BP_METATAG_INT(Priority, FField)                           // Integer input for all properties
BP_METATAG_FLOAT(Weight, FField)                           // Float input for all properties
BP_METATAG_LIST(Category, FObjectProperty, "UI", "Logic")  // Dropdown, only on object properties
```

The tags will automatically appear in the **Meta Tags** category of the Blueprint variable details panel.

### 3. Read meta tags at runtime

Meta tags are stored as standard UE property metadata. You can read them with:

```cpp
if (Property->HasMetaData("MyFlag"))
{
    FString Value = Property->GetMetaData("MyFlag");
}
```

## Macro Reference

All macros take a `Name` and a `FieldType` (an `FField` subclass that controls which property types the tag applies to).

| Macro | Editor Widget | Notes |
|-------|--------------|-------|
| `BP_METATAG_BOOL(Name, FieldType)` | Checkbox | No value editor — presence of the tag is the value |
| `BP_METATAG_STRING(Name, FieldType)` | Text box | Commits on Enter/focus loss |
| `BP_METATAG_INT(Name, FieldType)` | Numeric entry (int32) | Commits on Enter/focus loss |
| `BP_METATAG_FLOAT(Name, FieldType)` | Numeric entry (float) | Commits on Enter/focus loss |
| `BP_METATAG_LIST(Name, FieldType, ...)` | Dropdown | Options passed as variadic string arguments |


## Field Class Filtering

The `FieldType` parameter controls which Blueprint variables show the tag. Use any `FField` subclass:

- `FField` — all properties
- `FBoolProperty` — bool variables only
- `FObjectProperty` — object reference variables only
- `FNumericProperty` — int/float variables
- etc.

**Container support:** The filter also checks inner types of containers. An `FObjectProperty` filter will match:
- `UObject*` variables (direct match)
- `TArray<UObject*>` variables (array inner match)
- `TSet<UObject*>` variables (set element match)
- `TMap<..., UObject*>` variables (map key/value match)

## How It Works

1. `BP_METATAG_*` macros create static `FBlueprintableMetaTag` instances that auto-register into a global set
2. The plugin module registers a detail customization (`FBlueprintVariableMetaTagCustomization`) with the Blueprint editor
3. When a Blueprint variable is selected, the customization iterates all registered tags, filters by field class, and builds the appropriate Slate widgets
4. Edits are persisted to `FBPVariableDescription::SetMetaData()` and the Blueprint is marked modified

## Requirements

- Unreal Engine 5.7+
- Editor-only (the plugin loads at `PostEngineInit` phase, type `Editor`)
