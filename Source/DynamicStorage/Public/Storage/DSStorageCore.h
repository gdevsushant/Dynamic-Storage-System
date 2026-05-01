#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"

#include "DSStorageCore.generated.h"

/**
 * Raw byte storage core.
 *
 * This class does NOT know about data types.
 * It only stores serialized byte buffers against Gameplay Tags.
 */
UCLASS()
class DYNAMICSTORAGE_API UDSStorageCore : public UObject
{
    GENERATED_BODY()

public:

    /**
     * Stores raw byte data using a Gameplay Tag.
     */
    void SetRawValue(
        const FGameplayTag& Tag,
        const TArray<uint8>& Data
    );

    /**
     * Tries to read raw byte data using a Gameplay Tag.
     *
     * Returns true if data exists.
     */
    bool GetRawValue(
        const FGameplayTag& Tag,
        TArray<uint8>& OutData
    ) const;

    /**
     * Checks whether storage contains data for this tag.
     */
    bool HasValue(
        const FGameplayTag& Tag
    ) const;

    /**
     * Removes data for this tag.
     *
     * Returns true if something was removed.
     */
    bool RemoveValue(
        const FGameplayTag& Tag
    );

    /**
     * Clears all stored runtime data.
     */
    void ClearAll();

    /**
     * Returns number of stored entries.
     * Useful for testing/debugging.
     */
    int32 Num() const;

    void SetTypeId(
        const FGameplayTag& Tag,
        const FName& TypeId
    );

    bool GetTypeId(
        const FGameplayTag& Tag,
        FName& OutTypeId
    ) const;

    bool HasMatchingType(
        const FGameplayTag& Tag,
        const FName& RequestedTypeId
    ) const;

    void RemoveTypeId(
        const FGameplayTag& Tag
    );

    bool HasTypeId(
        const FGameplayTag& Tag
    ) const;

private:

    /**
     * Single runtime storage map.
     *
     * Key   = Gameplay Tag
     * Value = Serialized byte data
     */
    TMap<FGameplayTag, TArray<uint8>> StorageMap;
    TMap<FGameplayTag, FName> TypeMap;
};