#pragma once

#include "CoreMinimal.h"
#include <type_traits>

/**
 * Handles enum serialization.
 *
 * Supports enum class / enum types by storing their underlying numeric value.
 */
class DYNAMICSTORAGE_API FDSEnumSerializer
{
public:

    template<typename T>
    static constexpr bool IsSupported()
    {
        return std::is_enum_v<T>;
    }

    template<typename T>
    static bool Serialize(
        const T& Value,
        TArray<uint8>& OutBytes
    )
    {
        static_assert(
            std::is_enum_v<T>,
            "FDSEnumSerializer only supports enum types."
        );

        using UnderlyingType = std::underlying_type_t<T>;

        UnderlyingType RawValue = static_cast<UnderlyingType>(Value);

        OutBytes.SetNumUninitialized(sizeof(UnderlyingType));
        FMemory::Memcpy(OutBytes.GetData(), &RawValue, sizeof(UnderlyingType));

        return true;
    }

    template<typename T>
    static bool Deserialize(
        const TArray<uint8>& Bytes,
        T& OutValue
    )
    {
        static_assert(
            std::is_enum_v<T>,
            "FDSEnumSerializer only supports enum types."
        );

        using UnderlyingType = std::underlying_type_t<T>;

        if (Bytes.Num() != sizeof(UnderlyingType))
        {
            return false;
        }

        UnderlyingType RawValue{};
        FMemory::Memcpy(&RawValue, Bytes.GetData(), sizeof(UnderlyingType));

        OutValue = static_cast<T>(RawValue);
        return true;
    }
};