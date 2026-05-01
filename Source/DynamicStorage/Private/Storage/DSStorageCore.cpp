// Copyright (c) Sushant 2026. All Rights Reserved.

#include "Storage/DSStorageCore.h"
#include "UObject/NameTypes.h"

void UDSStorageCore::SetRawValue(
    const FGameplayTag& Tag,
    const TArray<uint8>& Data
)
{
    if (!Tag.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("DSStorageCore: Tried to set value with invalid GameplayTag."));
        return;
    }

    StorageMap.Add(Tag, Data);
}

bool UDSStorageCore::GetRawValue(
    const FGameplayTag& Tag,
    TArray<uint8>& OutData
) const
{
    if (!Tag.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("DSStorageCore: Tried to get value with invalid GameplayTag."));
        return false;
    }

    const TArray<uint8>* FoundData = StorageMap.Find(Tag);

    if (!FoundData)
    {
        return false;
    }

    OutData = *FoundData;
    return true;
}

bool UDSStorageCore::HasValue(
    const FGameplayTag& Tag
) const
{
    if (!Tag.IsValid())
    {
        return false;
    }

    return StorageMap.Contains(Tag);
}

bool UDSStorageCore::RemoveValue(
    const FGameplayTag& Tag
)
{
    if (!Tag.IsValid())
    {
        return false;
    }

    const bool bRemovedData = StorageMap.Remove(Tag) > 0;
    TypeMap.Remove(Tag);

    return bRemovedData;
}

void UDSStorageCore::ClearAll()
{
    StorageMap.Empty();
    TypeMap.Empty();
}

int32 UDSStorageCore::Num() const
{
    return StorageMap.Num();
}

void UDSStorageCore::SetTypeId(
    const FGameplayTag& Tag,
    const FName& TypeId
)
{
    if (!Tag.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("DSStorageCore: Tried to set type with invalid GameplayTag."));
        return;
    }

    TypeMap.Add(Tag, TypeId);
}

bool UDSStorageCore::GetTypeId(
    const FGameplayTag& Tag,
    FName& OutTypeId
) const
{
    if (!Tag.IsValid())
    {
        return false;
    }

    const FName* FoundType = TypeMap.Find(Tag);

    if (!FoundType)
    {
        return false;
    }

    OutTypeId = *FoundType;
    return true;
}

bool UDSStorageCore::HasMatchingType(
    const FGameplayTag& Tag,
    const FName& RequestedTypeId
) const
{
    FName StoredTypeId;

    if (!GetTypeId(Tag, StoredTypeId))
    {
        return false;
    }

    return StoredTypeId == RequestedTypeId;
}

void UDSStorageCore::RemoveTypeId(
    const FGameplayTag& Tag
)
{
    if (!Tag.IsValid())
    {
        return;
    }

    TypeMap.Remove(Tag);
}

bool UDSStorageCore::HasTypeId(
    const FGameplayTag& Tag
) const
{
    if (!Tag.IsValid())
    {
        return false;
    }

    return TypeMap.Contains(Tag);
}