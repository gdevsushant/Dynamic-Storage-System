#pragma once

#include "CoreMinimal.h"

/**
 * Handles FText serialization.
 *
 * Uses Unreal archive serialization directly so FText metadata/history
 * is preserved as much as Unreal's archive system supports.
 */
class DYNAMICSTORAGE_API FDSFTextSpecialCase
{
public:

    static void Serialize(
        const FText& Value,
        TArray<uint8>& OutBytes
    );

    static bool Deserialize(
        const TArray<uint8>& Bytes,
        FText& OutValue
    );
};