# Dynamic Storage System

**Dynamic Storage System** is an Unreal Engine plugin that lets you store and retrieve runtime data using **Gameplay Tags**.

It is designed for gameplay systems that need to share temporary or runtime values without directly depending on each other. For example, an input system can store a movement value, a weapon system can store current ammo, an AI system can store a target actor, and a UI system can read gameplay state using the same tag-based storage layer.

---

## Features

- Store and retrieve data using `FGameplayTag`
- Works in both **C++** and **Blueprint**
- Type-safe runtime retrieval
- Blueprint nodes for easy usage
- Automatic Blueprint pin type reconstruction
- Persistent Blueprint type metadata
- Supports common Unreal Engine types
- Supports arrays, sets, and maps
- Supports object and class references
- Safe handling for invalid tags, missing data, and type mismatches

---

## Blueprint Nodes

The plugin provides four main Blueprint nodes:

| Node | Purpose |
|---|---|
| **Set Storage** | Stores a value using a Gameplay Tag |
| **Get Storage** | Retrieves a value using a Gameplay Tag |
| **Contain Data** | Checks whether data exists for a tag |
| **Unregister Data** | Removes stored data for a tag |

---

## Basic Blueprint Usage

### Set Storage

Use **Set Storage** to save a value.

Example:

```text
Tag: Player.Health
Value: 100
```

This stores the integer value `100` under the tag `Player.Health`.

The `Value` pin automatically changes based on the connected value type.

---

### Get Storage

Use **Get Storage** to retrieve the value later.

Example:

```text
Tag: Player.Health
Output: int
```

The output pin automatically becomes the correct type when the tag has registered type metadata.

> **Note:** `Get Storage` should use a fixed Gameplay Tag or `Make Literal Gameplay Tag`, because Blueprint output pins need to know their type before compile time.

---

### Contain Data

Use **Contain Data** to check if a value exists before reading it.

Example flow:

```text
Contain Data → Branch → Get Storage
```

---

### Unregister Data

Use **Unregister Data** to remove stored data.

Example:

```text
Unregister Data: Player.Health
```

After removal, `Contain Data` for that tag returns `false`.

---

## C++ Usage

Include the API header:

```cpp
#include "API/DynamicStorageSystemAPI.h"
```

### Register Data

```cpp
UDynamicStorageSystemAPI::RegisterData<int32>(
    this,
    FGameplayTag::RequestGameplayTag(TEXT("Player.Health")),
    100
);
```

### Retrieve Data

```cpp
int32 Health = UDynamicStorageSystemAPI::RetrieveData<int32>(
    this,
    FGameplayTag::RequestGameplayTag(TEXT("Player.Health"))
);
```

### Check if Data Exists

```cpp
bool bExists = UDynamicStorageSystemAPI::ContainsData(
    this,
    FGameplayTag::RequestGameplayTag(TEXT("Player.Health"))
);
```

### Remove Data

```cpp
bool bRemoved = UDynamicStorageSystemAPI::UnregisterData(
    this,
    FGameplayTag::RequestGameplayTag(TEXT("Player.Health"))
);
```

---

## Supported Types

Dynamic Storage System supports many commonly used Unreal Engine types.

### Basic Types

```text
bool
int32
int64
float
double
```

### Text Types

```text
FString
FName
FText
```

### Math Types

```text
FVector
FVector2D
FRotator
FTransform
```

### Other Types

```text
Enums
Structs
UObject references
UClass references
Arrays
Sets
Maps
```

Example container types:

```text
TArray<int32>
TArray<FVector>
TArray<CustomStruct>
TSet<FString>
TMap<FName, int32>
TMap<FString, FVector>
TMap<FName, CustomStruct>
```

---

## Example Use Cases

### Player Runtime State

```text
Player.Health
Player.Stamina
Player.CurrentWeapon
```

### Weapon Systems

```text
Weapon.CurrentAmmo
Weapon.RuntimeData
Weapon.CurrentTarget
```

### Input Systems

```text
Input.Action.Move
Input.Action.MouseLocation
Input.Action.Look
```

### AI Systems

```text
AI.TargetActor
AI.LastKnownLocation
AI.CurrentState
```

### UI Systems

```text
UI.SelectedItem
UI.CurrentMenu
UI.DisplayedHealth
```

---

## Gameplay Tag Naming

A clear tag naming style is recommended:

```text
System.Category.Name
```

Good examples:

```text
Player.Stat.Health
Weapon.Data.CurrentAmmo
Input.Action.MouseLocation
AI.Target.CurrentActor
```

Avoid vague names such as:

```text
Data.Value
Test.Temp
Thing.One
```

---

## Type Safety

The stored type and requested type must match.

Correct:

```text
Store Player.Health as int
Get Player.Health as int
```

Incorrect:

```text
Store Player.Health as int
Get Player.Health as float
```

If a type mismatch occurs, the plugin blocks the retrieval and returns a default value instead of giving invalid data.

---

## Enhanced Input Recommendation

For Enhanced Input, store the resolved value from `FInputActionValue`, not the wrapper itself.

Recommended mapping:

```text
Boolean input  → bool
Axis1D input   → float
Axis2D input   → FVector2D
Axis3D input   → FVector
```

Example:

```cpp
FVector2D MoveValue = Value.Get<FVector2D>();

UDynamicStorageSystemAPI::RegisterData<FVector2D>(
    this,
    FGameplayTag::RequestGameplayTag(TEXT("Input.Action.Move")),
    MoveValue
);
```

---

## World Context

The plugin needs a valid world context object.

It works normally inside:

```text
Actor Blueprint
Actor Component Blueprint
Pawn Blueprint
Character Blueprint
Widget Blueprint
Player Controller Blueprint
```

If using it from a Blueprint that does not have a valid world context, pass a valid context object such as an Actor, Actor Component, Player Controller, Game Instance, or Widget.

---

## Common Issues

### Get Storage output pin is wildcard

**Cause:** The tag has no saved type metadata yet.

**Fix:** Use `Set Storage` with that tag, compile the Blueprint, then use `Get Storage` again.

---

### Type mismatch error

**Cause:** The value was stored as one type and retrieved as another.

**Fix:** Retrieve using the same type used when storing.

---

### Invalid world context

**Cause:** The plugin could not find a valid world or game instance.

**Fix:** Use the node inside an Actor, Component, Widget, or pass a valid context object manually.

---

### Dynamic tag does not work with Get Storage

**Cause:** Blueprint output pin type must be known before compile time.

**Fix:** Use a fixed Gameplay Tag or `Make Literal Gameplay Tag` for `Get Storage`.

---

## Best Practices

- Use clear Gameplay Tag names
- Do not store different types under the same tag
- Use `Contain Data` before reading optional values
- Use `Unregister Data` when temporary data is no longer needed
- Prefer fixed tags for Blueprint `Get Storage`
- Store simple resolved values for systems like Enhanced Input

---

## Limitations

- Runtime storage is not automatically a Save Game system
- Stored values are not automatically replicated in multiplayer
- Blueprint `Get Storage` needs a known tag to determine output type
- Some C++-only types cannot be represented directly in Blueprint

---

## Quick Reference

### Blueprint Nodes

```text
Set Storage
Get Storage
Contain Data
Unregister Data
```

### C++ API

```text
RegisterData<T>()
RetrieveData<T>()
ContainsData()
UnregisterData()
```

### Main Key Type

```text
Gameplay Tag
```

### Main Rule

```text
Store and retrieve using the same type.
```

---

## Status

Dynamic Storage System currently supports:

```text
C++ runtime storage
Blueprint Set/Get nodes
Primitive values
Struct values
Object/class references
Arrays
Sets
Maps
Type-safe retrieval
Blueprint metadata persistence
Editor pin reconstruction
Contain/Unregister operations
```

The plugin is intended for flexible runtime gameplay data sharing between systems while keeping those systems loosely coupled.
