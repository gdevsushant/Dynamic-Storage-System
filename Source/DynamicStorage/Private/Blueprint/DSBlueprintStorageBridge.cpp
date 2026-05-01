#include "Blueprint/DSBlueprintStorageBridge.h"

#include "API/DynamicStorageSystemAPI.h"
#include "DynamicStorage/Public/API/DynamicStorageSystemAPI.h"
#include "Serialization/Core/DSValueSerializer.h"
#include "UObject/UnrealType.h"

bool UDSBlueprintStorageBridge::RegisterDataByProperty(
    UObject* WorldContextObject,
    const FGameplayTag& Tag,
    FProperty* Property,
    const void* ValuePtr
)
{
    if (!WorldContextObject || !Tag.IsValid() || !Property || !ValuePtr)
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicStorage: RegisterDataByProperty failed. Invalid input."));
        return false;
    }

    UDynamicStorageSystemAPI* System =
        UDynamicStorageSystemAPI::Get(WorldContextObject);

    if (!System)
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicStorage: RegisterDataByProperty failed. Invalid system."));
        return false;
    }

    return System->RegisterDataByProperty_Internal(
        Tag,
        Property,
        ValuePtr
    );
}

bool UDSBlueprintStorageBridge::RetrieveDataByProperty(
    UObject* WorldContextObject,
    const FGameplayTag& Tag,
    FProperty* Property,
    void* OutValuePtr
)
{
    if (!WorldContextObject || !Tag.IsValid() || !Property || !OutValuePtr)
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicStorage: RetrieveDataByProperty failed. Invalid input."));
        return false;
    }

    UDynamicStorageSystemAPI* System =
        UDynamicStorageSystemAPI::Get(WorldContextObject);

    if (!System)
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicStorage: RetrieveDataByProperty failed. Invalid system."));
        return false;
    }

    return System->RetrieveDataByProperty_Internal(
        Tag,
        Property,
        OutValuePtr
    );
}

bool UDSBlueprintStorageBridge::ContainData(
    UObject* WorldContextObject,
    FGameplayTag Tag
)
{
    return UDynamicStorageSystemAPI::ContainsData(
        WorldContextObject,
        Tag
    );
}

bool UDSBlueprintStorageBridge::UnregisterData(
    UObject* WorldContextObject,
    FGameplayTag Tag
)
{
    return UDynamicStorageSystemAPI::UnregisterData(
        WorldContextObject,
        Tag
    );
}

void UDSBlueprintStorageBridge::RegisterBoolData(
    UObject* WorldContextObject,
    FGameplayTag Tag,
    bool Data
)
{
    UDynamicStorageSystemAPI::RegisterData<bool>(
        WorldContextObject,
        Tag,
        Data
    );
}

void UDSBlueprintStorageBridge::RegisterIntData(
    UObject* WorldContextObject,
    FGameplayTag Tag,
    int32 Data
)
{
    UDynamicStorageSystemAPI::RegisterData<int32>(
        WorldContextObject,
        Tag,
        Data
    );
}

void UDSBlueprintStorageBridge::RegisterFloatData(
    UObject* WorldContextObject,
    FGameplayTag Tag,
    float Data
)
{
    UDynamicStorageSystemAPI::RegisterData<float>(
        WorldContextObject,
        Tag,
        Data
    );
}

void UDSBlueprintStorageBridge::RegisterDoubleData(
    UObject* WorldContextObject,
    FGameplayTag Tag,
    double Data
)
{
    UDynamicStorageSystemAPI::RegisterData<double>(
        WorldContextObject,
        Tag,
        Data
    );
}

void UDSBlueprintStorageBridge::RegisterStringData(
    UObject* WorldContextObject,
    FGameplayTag Tag,
    FString Data
)
{
    UDynamicStorageSystemAPI::RegisterData<FString>(
        WorldContextObject,
        Tag,
        Data
    );
}

void UDSBlueprintStorageBridge::RegisterNameData(
    UObject* WorldContextObject,
    FGameplayTag Tag,
    FName Data
)
{
    UDynamicStorageSystemAPI::RegisterData<FName>(
        WorldContextObject,
        Tag,
        Data
    );
}

void UDSBlueprintStorageBridge::RegisterTextData(
    UObject* WorldContextObject,
    FGameplayTag Tag,
    FText Data
)
{
    UDynamicStorageSystemAPI::RegisterData<FText>(
        WorldContextObject,
        Tag,
        Data
    );
}

void UDSBlueprintStorageBridge::RegisterByteData(
    UObject* WorldContextObject,
    FGameplayTag Tag,
    uint8 Data
)
{
    UDynamicStorageSystemAPI::RegisterData<uint8>(
        WorldContextObject,
        Tag,
        Data
    );
}

void UDSBlueprintStorageBridge::RegisterObjectData(
    UObject* WorldContextObject,
    FGameplayTag Tag,
    UObject* Data
)
{
    UDynamicStorageSystemAPI::RegisterData<UObject*>(
        WorldContextObject,
        Tag,
        Data
    );
}

void UDSBlueprintStorageBridge::RegisterClassData(
    UObject* WorldContextObject,
    FGameplayTag Tag,
    UClass* Data
)
{
    UDynamicStorageSystemAPI::RegisterData<UClass*>(
        WorldContextObject,
        Tag,
        Data
    );
}

void UDSBlueprintStorageBridge::RegisterStructData(
    UObject* WorldContextObject,
    FGameplayTag Tag,
    const int32& Data
)
{
    // Never called directly. CustomThunk handles execution.
    checkNoEntry();
}

DEFINE_FUNCTION(UDSBlueprintStorageBridge::execRegisterStructData)
{
    P_GET_OBJECT(UObject, WorldContextObject);
    P_GET_STRUCT(FGameplayTag, Tag);

    Stack.StepCompiledIn<FProperty>(nullptr);

    FProperty* DataProperty = Stack.MostRecentProperty;
    void* DataPtr = Stack.MostRecentPropertyAddress;

    P_FINISH;

    if (!WorldContextObject || !Tag.IsValid() || !DataProperty || !DataPtr)
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicStorage: RegisterStructData failed. Invalid input."));
        return;
    }

    FStructProperty* StructProperty = CastField<FStructProperty>(DataProperty);

    if (!StructProperty || !StructProperty->Struct)
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicStorage: RegisterStructData failed. Data is not a struct."));
        return;
    }

    UDynamicStorageSystemAPI* System =
        UDynamicStorageSystemAPI::Get(WorldContextObject);

    if (!System)
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicStorage: RegisterStructData failed. Invalid storage system."));
        return;
    }

    if (!System->RegisterStructData_Internal(
        Tag,
        StructProperty->Struct,
        DataPtr
    ))
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicStorage: RegisterStructData failed for Tag [%s]."), *Tag.ToString());
    }
}

void UDSBlueprintStorageBridge::RegisterMapData(
    UObject* WorldContextObject,
    FGameplayTag Tag,
    const int32& Data
)
{
    checkNoEntry();
}

DEFINE_FUNCTION(UDSBlueprintStorageBridge::execRegisterMapData)
{
    P_GET_OBJECT(UObject, WorldContextObject);
    P_GET_STRUCT(FGameplayTag, Tag);

    Stack.StepCompiledIn<FProperty>(nullptr);

    FProperty* DataProperty = Stack.MostRecentProperty;
    void* DataPtr = Stack.MostRecentPropertyAddress;

    P_FINISH;

    if (!WorldContextObject || !Tag.IsValid() || !DataProperty || !DataPtr)
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicStorage: RegisterMapData failed. Invalid input."));
        return;
    }

    FMapProperty* MapProperty = CastField<FMapProperty>(DataProperty);

    if (!MapProperty)
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicStorage: RegisterMapData failed. Data is not a map."));
        return;
    }

    UDynamicStorageSystemAPI* System =
        UDynamicStorageSystemAPI::Get(WorldContextObject);

    if (!System)
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicStorage: RegisterMapData failed. Invalid storage system."));
        return;
    }

    if (!System->RegisterBlueprintPropertyData_Internal(
        Tag,
        MapProperty,
        DataPtr
    ))
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("DynamicStorage: RegisterMapData failed for Tag [%s]."),
            *Tag.ToString()
        );
    }
}

void UDSBlueprintStorageBridge::RegisterArrayData(
    UObject* WorldContextObject,
    FGameplayTag Tag,
    const int32& Data
)
{
    checkNoEntry();
}

DEFINE_FUNCTION(UDSBlueprintStorageBridge::execRegisterArrayData)
{
    P_GET_OBJECT(UObject, WorldContextObject);
    P_GET_STRUCT(FGameplayTag, Tag);

    Stack.StepCompiledIn<FProperty>(nullptr);

    FProperty* DataProperty = Stack.MostRecentProperty;
    void* DataPtr = Stack.MostRecentPropertyAddress;

    P_FINISH;

    if (!WorldContextObject || !Tag.IsValid() || !DataProperty || !DataPtr)
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicStorage: RegisterArrayData failed. Invalid input."));
        return;
    }

    FArrayProperty* ArrayProperty = CastField<FArrayProperty>(DataProperty);

    if (!ArrayProperty)
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicStorage: RegisterArrayData failed. Data is not an array."));
        return;
    }

    UDynamicStorageSystemAPI* System =
        UDynamicStorageSystemAPI::Get(WorldContextObject);

    if (!System)
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicStorage: RegisterArrayData failed. Invalid storage system."));
        return;
    }

    if (!System->RegisterBlueprintPropertyData_Internal(
        Tag,
        ArrayProperty,
        DataPtr
    ))
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("DynamicStorage: RegisterArrayData failed for Tag [%s]."),
            *Tag.ToString()
        );
    }
}


void UDSBlueprintStorageBridge::RegisterSetData(
    UObject* WorldContextObject,
    FGameplayTag Tag,
    const int32& Data
)
{
    checkNoEntry();
}

DEFINE_FUNCTION(UDSBlueprintStorageBridge::execRegisterSetData)
{
    P_GET_OBJECT(UObject, WorldContextObject);
    P_GET_STRUCT(FGameplayTag, Tag);

    Stack.StepCompiledIn<FProperty>(nullptr);

    FProperty* DataProperty = Stack.MostRecentProperty;
    void* DataPtr = Stack.MostRecentPropertyAddress;

    P_FINISH;

    if (!WorldContextObject || !Tag.IsValid() || !DataProperty || !DataPtr)
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicStorage: RegisterSetData failed. Invalid input."));
        return;
    }

    FSetProperty* SetProperty = CastField<FSetProperty>(DataProperty);

    if (!SetProperty)
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicStorage: RegisterSetData failed. Data is not a set."));
        return;
    }

    UDynamicStorageSystemAPI* System =
        UDynamicStorageSystemAPI::Get(WorldContextObject);

    if (!System)
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicStorage: RegisterSetData failed. Invalid storage system."));
        return;
    }

    if (!System->RegisterBlueprintPropertyData_Internal(
        Tag,
        SetProperty,
        DataPtr
    ))
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("DynamicStorage: RegisterSetData failed for Tag [%s]."),
            *Tag.ToString()
        );
    }
}

bool UDSBlueprintStorageBridge::RetrieveBoolData(
    UObject* WorldContextObject,
    FGameplayTag Tag
)
{
    return UDynamicStorageSystemAPI::RetrieveData<bool>(
        WorldContextObject,
        Tag
    );
}

int32 UDSBlueprintStorageBridge::RetrieveIntData(
    UObject* WorldContextObject,
    FGameplayTag Tag
)
{
    return UDynamicStorageSystemAPI::RetrieveData<int32>(
        WorldContextObject,
        Tag
    );
}

float UDSBlueprintStorageBridge::RetrieveFloatData(
    UObject* WorldContextObject,
    FGameplayTag Tag
)
{
    return UDynamicStorageSystemAPI::RetrieveData<float>(
        WorldContextObject,
        Tag
    );
}

double UDSBlueprintStorageBridge::RetrieveDoubleData(
    UObject* WorldContextObject,
    FGameplayTag Tag
)
{
    return UDynamicStorageSystemAPI::RetrieveData<double>(
        WorldContextObject,
        Tag
    );
}

FString UDSBlueprintStorageBridge::RetrieveStringData(
    UObject* WorldContextObject,
    FGameplayTag Tag
)
{
    return UDynamicStorageSystemAPI::RetrieveData<FString>(
        WorldContextObject,
        Tag
    );
}

FName UDSBlueprintStorageBridge::RetrieveNameData(
    UObject* WorldContextObject,
    FGameplayTag Tag
)
{
    return UDynamicStorageSystemAPI::RetrieveData<FName>(
        WorldContextObject,
        Tag
    );
}

FText UDSBlueprintStorageBridge::RetrieveTextData(
    UObject* WorldContextObject,
    FGameplayTag Tag
)
{
    return UDynamicStorageSystemAPI::RetrieveData<FText>(
        WorldContextObject,
        Tag
    );
}

uint8 UDSBlueprintStorageBridge::RetrieveByteData(
    UObject* WorldContextObject,
    FGameplayTag Tag
)
{
    return UDynamicStorageSystemAPI::RetrieveData<uint8>(
        WorldContextObject,
        Tag
    );
}

UObject* UDSBlueprintStorageBridge::RetrieveObjectData(
    UObject* WorldContextObject,
    FGameplayTag Tag
)
{
    return UDynamicStorageSystemAPI::RetrieveData<UObject*>(
        WorldContextObject,
        Tag
    );
}

UClass* UDSBlueprintStorageBridge::RetrieveClassData(
    UObject* WorldContextObject,
    FGameplayTag Tag
)
{
    return UDynamicStorageSystemAPI::RetrieveData<UClass*>(
        WorldContextObject,
        Tag
    );
}

void UDSBlueprintStorageBridge::RetrievePropertyData(
    UObject* WorldContextObject,
    FGameplayTag Tag,
    int32& Data
)
{
    checkNoEntry();
}

DEFINE_FUNCTION(UDSBlueprintStorageBridge::execRetrievePropertyData)
{
    P_GET_OBJECT(UObject, WorldContextObject);
    P_GET_STRUCT(FGameplayTag, Tag);

    Stack.StepCompiledIn<FProperty>(nullptr);

    FProperty* DataProperty = Stack.MostRecentProperty;
    void* DataPtr = Stack.MostRecentPropertyAddress;

    P_FINISH;

    if (!WorldContextObject || !Tag.IsValid() || !DataProperty || !DataPtr)
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicStorage: RetrievePropertyData failed. Invalid input."));
        return;
    }

    UDynamicStorageSystemAPI* System =
        UDynamicStorageSystemAPI::Get(WorldContextObject);

    if (!System)
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicStorage: RetrievePropertyData failed. Invalid storage system."));
        return;
    }

    if (!System->RetrieveBlueprintPropertyData_Internal(
        Tag,
        DataProperty,
        DataPtr
    ))
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("DynamicStorage: RetrievePropertyData failed for Tag [%s]."),
            *Tag.ToString()
        );
    }
}

void UDSBlueprintStorageBridge::RegisterInt64Data(
    UObject* WorldContextObject,
    FGameplayTag Tag,
    int64 Data
)
{
    UDynamicStorageSystemAPI::RegisterData<int64>(
        WorldContextObject,
        Tag,
        Data
    );
}

int64 UDSBlueprintStorageBridge::RetrieveInt64Data(
    UObject* WorldContextObject,
    FGameplayTag Tag
)
{
    return UDynamicStorageSystemAPI::RetrieveData<int64>(
        WorldContextObject,
        Tag
    );
}