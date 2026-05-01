// Copyright (c) Sushant 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <type_traits>

/**
 * Handles Unreal math/value types that should be stored as raw bytes.
 *
 * These types are value-style types and should not be forced through StaticStruct().
 */
class DYNAMICSTORAGE_API FDSMathTypeSerializer
{
public:

    template<typename T>
    static constexpr bool IsSupported()
    {
        return
            std::is_same_v<T, FVector> ||
            std::is_same_v<T, FVector2D> ||
            std::is_same_v<T, FVector4> ||
            std::is_same_v<T, FRotator> ||
            std::is_same_v<T, FQuat> ||
            std::is_same_v<T, FTransform> ||
            std::is_same_v<T, FColor> ||
            std::is_same_v<T, FLinearColor> ||
            std::is_same_v<T, FIntPoint> ||
            std::is_same_v<T, FIntVector>;
    }

    template<typename T>
    static void Serialize(
        const T& Value,
        TArray<uint8>& OutBytes
    )
    {
        static_assert(
            IsSupported<T>(),
            "FDSMathTypeSerializer only supports Unreal math/value types."
        );

        OutBytes.SetNumUninitialized(sizeof(T));
        FMemory::Memcpy(OutBytes.GetData(), &Value, sizeof(T));
    }

    template<typename T>
    static bool Deserialize(
        const TArray<uint8>& Bytes,
        T& OutValue
    )
    {
        static_assert(
            IsSupported<T>(),
            "FDSMathTypeSerializer only supports Unreal math/value types."
        );

        if (Bytes.Num() != sizeof(T))
        {
            return false;
        }

        FMemory::Memcpy(&OutValue, Bytes.GetData(), sizeof(T));
        return true;
    }
};