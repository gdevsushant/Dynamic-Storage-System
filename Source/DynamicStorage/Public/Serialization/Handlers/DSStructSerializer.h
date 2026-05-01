// Copyright (c) Sushant 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <type_traits>

/**
 * Runtime trait:
 * Detects whether a C++ type has StaticStruct().
 *
 * This is intentionally separate from DynamicStorageSystemAPI metadata traits.
 */
template<typename T, typename = void>
struct TDSStructHasStaticStruct
{
    static constexpr bool Value = false;
};

template<typename T>
struct TDSStructHasStaticStruct<T, std::void_t<decltype(T::StaticStruct())>>
{
    static constexpr bool Value = true;
};

class DYNAMICSTORAGE_API FDSStructSerializer
{
public:

    template<typename T>
    static bool Serialize(
        const T& Value,
        TArray<uint8>& OutBytes
    )
    {
        using CleanT = std::remove_cv_t<std::remove_reference_t<T>>;

        if constexpr (TDSStructHasStaticStruct<CleanT>::Value)
        {
            return SerializeByStruct(
                CleanT::StaticStruct(),
                &Value,
                OutBytes
            );
        }
        else
        {
            return false;
        }
    }

    template<typename T>
    static bool Deserialize(
        const TArray<uint8>& Bytes,
        T& OutValue
    )
    {
        using CleanT = std::remove_cv_t<std::remove_reference_t<T>>;

        if constexpr (TDSStructHasStaticStruct<CleanT>::Value)
        {
            return DeserializeByStruct(
                CleanT::StaticStruct(),
                Bytes,
                &OutValue
            );
        }
        else
        {
            return false;
        }
    }

    static bool SerializeByStruct(
        UScriptStruct* StructType,
        const void* StructData,
        TArray<uint8>& OutBytes
    );

    static bool DeserializeByStruct(
        UScriptStruct* StructType,
        const TArray<uint8>& Bytes,
        void* StructData
    );
};