// Copyright (c) Sushant 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Handles FName serialization.
 *
 * FName is stored as FString for safety and portability.
 */
class DYNAMICSTORAGE_API FDSNameSpecialCase
{
public:

    static void Serialize(
        const FName& Value,
        TArray<uint8>& OutBytes
    );

    static bool Deserialize(
        const TArray<uint8>& Bytes,
        FName& OutValue
    );
};