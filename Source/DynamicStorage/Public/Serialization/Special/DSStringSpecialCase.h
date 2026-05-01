#pragma once

#include "CoreMinimal.h"

/**
 * Handles FString serialization.
 *
 * FString must NOT be stored with raw memcpy.
 */
class DYNAMICSTORAGE_API FDSStringSpecialCase
{
public:

    static void Serialize(
        const FString& Value,
        TArray<uint8>& OutBytes
    );

    static bool Deserialize(
        const TArray<uint8>& Bytes,
        FString& OutValue
    );
};