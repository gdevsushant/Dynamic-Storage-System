// Copyright (c) Sushant 2026. All Rights Reserved.

#include "API/DynamicStorageSystemAPI.h"
#include "Storage/DSStorageCore.h"
#include "Kismet/GameplayStatics.h"
#include "Storage/DSObjectRegistry.h"
#include "Serialization/Handlers/DSStructSerializer.h"
#include "Blueprint/DSBlueprintPropertySerializer.h"

//////////////////////////////////////////////////////////////
// 🔹 GET SYSTEM
//////////////////////////////////////////////////////////////

UDynamicStorageSystemAPI* UDynamicStorageSystemAPI::Get(UObject* WorldContextObject)
{
    if (!WorldContextObject)
    {
        return nullptr;
    }

    UGameInstance* GI = UGameplayStatics::GetGameInstance(WorldContextObject);

    if (!GI)
    {
        return nullptr;
    }

    return GI->GetSubsystem<UDynamicStorageSystemAPI>();
}

//////////////////////////////////////////////////////////////
// 🔹 INITIALIZE
//////////////////////////////////////////////////////////////

void UDynamicStorageSystemAPI::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    StorageCore = NewObject<UDSStorageCore>(this);

    if (!StorageCore)
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicStorage: Failed to create StorageCore"));
    }

    ObjectRegistry = NewObject<UDSObjectRegistry>(this);

    if (!ObjectRegistry)
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicStorage: Failed to create ObjectRegistry"));
    }
}


//////////////////////////////////////////////////////////////
// 🔹 DEINITIALIZE
//////////////////////////////////////////////////////////////

void UDynamicStorageSystemAPI::Deinitialize()
{
    StorageCore = nullptr;
    ObjectRegistry = nullptr;

    Super::Deinitialize();
}

//////////////////////////////////////////////////////////////
// 🔹 HAS VALUE
//////////////////////////////////////////////////////////////

bool UDynamicStorageSystemAPI::ContainsData(
    UObject* WorldContextObject,
    const FGameplayTag& Tag
)
{
    UDynamicStorageSystemAPI* System = Get(WorldContextObject);

    if (!System || !System->StorageCore)
    {
        return false;
    }

    return System->StorageCore->HasValue(Tag);
}

//////////////////////////////////////////////////////////////
// 🔹 REMOVE VALUE
//////////////////////////////////////////////////////////////

bool UDynamicStorageSystemAPI::UnregisterData(
    UObject* WorldContextObject,
    const FGameplayTag& Tag
)
{
    if (!Tag.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicStorage: UnregisterData failed. Invalid GameplayTag."));
        ensureMsgf(false, TEXT("DynamicStorage: UnregisterData failed. Invalid GameplayTag."));
        return false;
    }

    UDynamicStorageSystemAPI* System = Get(WorldContextObject);

    if (!System || !System->StorageCore)
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicStorage: UnregisterData failed. Invalid storage system."));
        ensureMsgf(false, TEXT("DynamicStorage: UnregisterData failed. Invalid storage system."));
        return false;
    }

    if (!System->StorageCore->HasValue(Tag))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("DynamicStorage: UnregisterData called for missing Tag [%s]."),
            *Tag.ToString()
        );

        return false;
    }

    return System->StorageCore->RemoveValue(Tag);
}

bool UDynamicStorageSystemAPI::RegisterDataByProperty_Internal(
    const FGameplayTag& Tag,
    FProperty* Property,
    const void* ValuePtr
)
{
    UE_LOG(LogTemp, Warning, TEXT("DynamicStorage: RegisterDataByProperty_Internal is reserved for K2 node implementation."));
    return false;
}

bool UDynamicStorageSystemAPI::RetrieveDataByProperty_Internal(
    const FGameplayTag& Tag,
    FProperty* Property,
    void* OutValuePtr
)
{
    UE_LOG(LogTemp, Warning, TEXT("DynamicStorage: RetrieveDataByProperty_Internal is reserved for K2 node implementation."));
    return false;
}

bool UDynamicStorageSystemAPI::RegisterStructData_Internal(
    const FGameplayTag& Tag,
    UScriptStruct* StructType,
    const void* StructData
)
{
    if (!Tag.IsValid() || !StructType || !StructData || !StorageCore)
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicStorage: RegisterStructData_Internal failed. Invalid input."));
        ensureMsgf(false, TEXT("DynamicStorage: RegisterStructData_Internal failed. Invalid input."));
        return false;
    }

    TArray<uint8> Bytes;

    if (!FDSStructSerializer::SerializeByStruct(
        StructType,
        StructData,
        Bytes
    ))
    {
        UE_LOG(LogTemp, Error, TEXT("DynamicStorage: Failed to serialize struct for Tag [%s]."), *Tag.ToString());
        ensureMsgf(false, TEXT("DynamicStorage: Failed to serialize struct."));
        return false;
    }

    StorageCore->SetRawValue(Tag, Bytes);
    StorageCore->SetTypeId(Tag, StructType->GetFName());

    return true;
}

bool UDynamicStorageSystemAPI::RegisterBlueprintPropertyData_Internal(
    const FGameplayTag& Tag,
    FProperty* Property,
    const void* DataPtr
)
{
    if (!Tag.IsValid() || !Property || !DataPtr || !StorageCore)
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("DynamicStorage: RegisterBlueprintPropertyData_Internal failed. Invalid input.")
        );

        ensureMsgf(
            false,
            TEXT("DynamicStorage: RegisterBlueprintPropertyData_Internal failed. Invalid input.")
        );

        return false;
    }

    TArray<uint8> Bytes;

    if (!FDSBlueprintPropertySerializer::SerializeProperty(
        Property,
        DataPtr,
        Bytes,
        ObjectRegistry
    ))
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("DynamicStorage: Failed to serialize Blueprint property for Tag [%s]."),
            *Tag.ToString()
        );

        ensureMsgf(
            false,
            TEXT("DynamicStorage: Failed to serialize Blueprint property.")
        );

        return false;
    }

    StorageCore->SetRawValue(Tag, Bytes);

    const FString TypeIdString =
        FString::Printf(
            TEXT("BlueprintProperty:%s"),
            *Property->GetCPPType()
        );

    StorageCore->SetTypeId(
        Tag,
        FName(*TypeIdString)
    );
    
    return true;
}

bool UDynamicStorageSystemAPI::RetrieveBlueprintPropertyData_Internal(
    const FGameplayTag& Tag,
    FProperty* Property,
    void* OutDataPtr
)
{
    if (!Tag.IsValid() || !Property || !OutDataPtr || !StorageCore)
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("DynamicStorage: RetrieveBlueprintPropertyData_Internal failed. Invalid input.")
        );

        ensureMsgf(
            false,
            TEXT("DynamicStorage: RetrieveBlueprintPropertyData_Internal failed. Invalid input.")
        );

        return false;
    }

    TArray<uint8> Bytes;

    if (!StorageCore->GetRawValue(Tag, Bytes))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("DynamicStorage: RetrieveBlueprintPropertyData_Internal failed. No raw data for Tag [%s]."),
            *Tag.ToString()
        );

        return false;
    }

    if (!FDSBlueprintPropertySerializer::DeserializeProperty(
        Property,
        OutDataPtr,
        Bytes,
        ObjectRegistry
    ))
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("DynamicStorage: Failed to deserialize Blueprint property for Tag [%s]."),
            *Tag.ToString()
        );

        ensureMsgf(
            false,
            TEXT("DynamicStorage: Failed to deserialize Blueprint property.")
        );

        return false;
    }

    return true;
}
     