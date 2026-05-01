// Copyright (c) Sushant 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <type_traits>

/**
 * Handles serialization for basic primitive value types.
 *
 * Supported:
 * - int32
 * - int64
 * - float
 * - double
 * - bool
 */
class DYNAMICSTORAGE_API FDSPrimitiveSerializer
{
public:

    template<typename T>
    static constexpr bool IsSupported()
    {
        return
            std::is_same_v<T, int32> ||
            std::is_same_v<T, int64> ||
            std::is_same_v<T, float> ||
            std::is_same_v<T, double> ||
            std::is_same_v<T, bool>;
    }

    template<typename T>
    static void Serialize(
        const T& Value,
        TArray<uint8>& OutBytes
    )
    {
        static_assert(
            std::is_same_v<T, int32> ||
            std::is_same_v<T, int64> ||
            std::is_same_v<T, float> ||
            std::is_same_v<T, double>||
            std::is_same_v<T, bool>,
            "FDSPrimitiveSerializer only supports int32, float, double, and bool."
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
            std::is_same_v<T, int32> ||
            std::is_same_v<T, int64> ||
            std::is_same_v<T, float> ||
            std::is_same_v<T, double> ||
            std::is_same_v<T, bool>,
            "FDSPrimitiveSerializer only supports int32, float, double, and bool."
        );

        if (Bytes.Num() != sizeof(T))
        {
            return false;
        }

        FMemory::Memcpy(&OutValue, Bytes.GetData(), sizeof(T));
        return true;
    }
};