#pragma once

#include "CoreMinimal.h"

#include <type_traits>

#include "Serialization/Core/DSCustomSerializer.h"

#include "Serialization/Handlers/DSPrimitiveSerializer.h"
#include "Serialization/Handlers/DSMathTypeSerializer.h"
#include "Serialization/Handlers/DSStructSerializer.h"
#include "Serialization/Handlers/DSEnumSerializer.h"
#include "Serialization/Handlers/DSObjectSerializer.h"
#include "Serialization/Handlers/DSClassSerializer.h"
#include "Serialization/Handlers/DSContainerSerializer.h"

#include "Serialization/Special/DSStringSpecialCase.h"
#include "Serialization/Special/DSNameSpecialCase.h"
#include "Serialization/Special/DSFTextSpecialCase.h"

class UDSObjectRegistry;

class DYNAMICSTORAGE_API FDSValueSerializer
{
public:

    static constexpr int32 MaxSerializationDepth = 64;

    template<typename T>
    static constexpr bool IsRawBinarySupported()
    {
        using CleanT = std::remove_cv_t<std::remove_reference_t<T>>;

        return
            std::is_trivially_copyable_v<CleanT> &&
            !std::is_pointer_v<CleanT> &&
            !std::is_enum_v<CleanT> &&

            !std::is_same_v<CleanT, int32> &&
            !std::is_same_v<CleanT, int64> &&
            !std::is_same_v<CleanT, float> &&
            !std::is_same_v<CleanT, double> &&
            !std::is_same_v<CleanT, bool> &&

            !std::is_same_v<CleanT, FString> &&
            !std::is_same_v<CleanT, FName> &&
            !std::is_same_v<CleanT, FText> &&

            !FDSMathTypeSerializer::IsSupported<CleanT>() &&
            !FDSContainerSerializer::IsArray<CleanT>() &&
            !FDSContainerSerializer::IsSet<CleanT>() &&
            !FDSContainerSerializer::IsMap<CleanT>() &&

            // Very important:
            // Reflected USTRUCTs must never go through raw binary.
            !TDSStructHasStaticStruct<CleanT>::Value;
    }

    template<typename T>
    static bool Serialize(
        const T& Value,
        TArray<uint8>& OutBytes,
        UDSObjectRegistry* ObjectRegistry = nullptr,
        int32 CurrentDepth = 0
    )
    {
        using CleanT = std::remove_cv_t<std::remove_reference_t<T>>;

        if (CurrentDepth > MaxSerializationDepth)
        {
            UE_LOG(LogTemp, Error, TEXT("DynamicStorage: Serialization depth limit exceeded."));
            ensureMsgf(false, TEXT("DynamicStorage: Serialization depth limit exceeded."));
            return false;
        }

        // =====================================================
        // Primitive
        // =====================================================

        if constexpr (
            std::is_same_v<CleanT, int32> ||
            std::is_same_v<CleanT, int64> ||
            std::is_same_v<CleanT, float> ||
            std::is_same_v<CleanT, double> ||
            std::is_same_v<CleanT, bool>
        )
        {
            FDSPrimitiveSerializer::Serialize<CleanT>(Value, OutBytes);
            return true;
        }

        // =====================================================
        // Enum
        // =====================================================

        else if constexpr (FDSEnumSerializer::IsSupported<CleanT>())
        {
            return FDSEnumSerializer::Serialize<CleanT>(Value, OutBytes);
        }

        // =====================================================
        // UObject / UClass
        // =====================================================

        else if constexpr (FDSObjectSerializer::IsSupported<CleanT>())
        {
            return FDSObjectSerializer::Serialize<CleanT>(
                Value,
                OutBytes,
                ObjectRegistry
            );
        }
        else if constexpr (FDSClassSerializer::IsSupported<CleanT>())
        {
            return FDSClassSerializer::Serialize<CleanT>(
                Value,
                OutBytes
            );
        }

        // =====================================================
        // Containers
        // =====================================================

        else if constexpr (FDSContainerSerializer::IsArray<CleanT>())
        {
            return FDSContainerSerializer::SerializeArray(
                Value,
                OutBytes,
                [ObjectRegistry, CurrentDepth](const auto& Element, TArray<uint8>& ElementBytes) -> bool
                {
                    using ElementType = std::decay_t<decltype(Element)>;

                    return FDSValueSerializer::Serialize<ElementType>(
                        Element,
                        ElementBytes,
                        ObjectRegistry,
                        CurrentDepth + 1
                    );
                }
            );
        }
        else if constexpr (FDSContainerSerializer::IsSet<CleanT>())
        {
            return FDSContainerSerializer::SerializeSet(
                Value,
                OutBytes,
                [ObjectRegistry, CurrentDepth](const auto& Element, TArray<uint8>& ElementBytes) -> bool
                {
                    using ElementType = std::decay_t<decltype(Element)>;

                    return FDSValueSerializer::Serialize<ElementType>(
                        Element,
                        ElementBytes,
                        ObjectRegistry,
                        CurrentDepth + 1
                    );
                }
            );
        }
        else if constexpr (FDSContainerSerializer::IsMap<CleanT>())
        {
            return FDSContainerSerializer::SerializeMap(
                Value,
                OutBytes,
                [ObjectRegistry, CurrentDepth](const auto& Key, TArray<uint8>& KeyBytes) -> bool
                {
                    using KeyType = std::decay_t<decltype(Key)>;

                    return FDSValueSerializer::Serialize<KeyType>(
                        Key,
                        KeyBytes,
                        ObjectRegistry,
                        CurrentDepth + 1
                    );
                },
                [ObjectRegistry, CurrentDepth](const auto& MapValue, TArray<uint8>& ValueBytes) -> bool
                {
                    using ValueType = std::decay_t<decltype(MapValue)>;

                    return FDSValueSerializer::Serialize<ValueType>(
                        MapValue,
                        ValueBytes,
                        ObjectRegistry,
                        CurrentDepth + 1
                    );
                }
            );
        }

        // =====================================================
        // Unreal string/name/text
        // =====================================================

        else if constexpr (std::is_same_v<CleanT, FString>)
        {
            FDSStringSpecialCase::Serialize(Value, OutBytes);
            return true;
        }
        else if constexpr (std::is_same_v<CleanT, FName>)
        {
            FDSNameSpecialCase::Serialize(Value, OutBytes);
            return true;
        }
        else if constexpr (std::is_same_v<CleanT, FText>)
        {
            FDSFTextSpecialCase::Serialize(Value, OutBytes);
            return true;
        }

        // =====================================================
        // Math types
        // =====================================================

        else if constexpr (FDSMathTypeSerializer::IsSupported<CleanT>())
        {
            FDSMathTypeSerializer::Serialize<CleanT>(Value, OutBytes);
            return true;
        }

        // =====================================================
        // Reflected USTRUCT
        //
        // Must come before raw binary.
        // =====================================================

        else if constexpr (TDSStructHasStaticStruct<CleanT>::Value)
        {
            return FDSStructSerializer::Serialize<CleanT>(
                Value,
                OutBytes
            );
        }

        // =====================================================
        // Custom serializers
        //
        // Must come before raw binary so custom types can override.
        // =====================================================

        else if constexpr (TDSCustomSerializer<CleanT>::bSupported)
        {
            return TDSCustomSerializer<CleanT>::Serialize(
                Value,
                OutBytes
            );
        }

        // =====================================================
        // Raw binary fallback for safe C++ POD types only
        // =====================================================

        else if constexpr (IsRawBinarySupported<CleanT>())
        {
            OutBytes.SetNumUninitialized(sizeof(CleanT));
            FMemory::Memcpy(OutBytes.GetData(), &Value, sizeof(CleanT));
            return true;
        }

        // =====================================================
        // Unsupported
        // =====================================================

        else
        {
            UE_LOG(
                LogTemp,
                Error,
                TEXT("DynamicStorage: Unsupported type for serialization.")
            );

            return false;
        }
    }

    template<typename T>
    static bool Deserialize(
        const TArray<uint8>& Bytes,
        T& OutValue,
        UDSObjectRegistry* ObjectRegistry = nullptr,
        int32 CurrentDepth = 0
    )
    {
        using CleanT = std::remove_cv_t<std::remove_reference_t<T>>;

        if (CurrentDepth > MaxSerializationDepth)
        {
            UE_LOG(LogTemp, Error, TEXT("DynamicStorage: Deserialization depth limit exceeded."));
            ensureMsgf(false, TEXT("DynamicStorage: Deserialization depth limit exceeded."));
            return false;
        }

        // =====================================================
        // Primitive
        // =====================================================

        if constexpr (
            std::is_same_v<CleanT, int32> ||
            std::is_same_v<CleanT, int64> ||
            std::is_same_v<CleanT, float> ||
            std::is_same_v<CleanT, double> ||
            std::is_same_v<CleanT, bool>
        )
        {
            return FDSPrimitiveSerializer::Deserialize<CleanT>(
                Bytes,
                OutValue
            );
        }

        // =====================================================
        // Enum
        // =====================================================

        else if constexpr (FDSEnumSerializer::IsSupported<CleanT>())
        {
            return FDSEnumSerializer::Deserialize<CleanT>(
                Bytes,
                OutValue
            );
        }

        // =====================================================
        // UObject / UClass
        // =====================================================

        else if constexpr (FDSObjectSerializer::IsSupported<CleanT>())
        {
            return FDSObjectSerializer::Deserialize<CleanT>(
                Bytes,
                OutValue,
                ObjectRegistry
            );
        }
        else if constexpr (FDSClassSerializer::IsSupported<CleanT>())
        {
            return FDSClassSerializer::Deserialize<CleanT>(
                Bytes,
                OutValue
            );
        }

        // =====================================================
        // Containers
        // =====================================================

        else if constexpr (FDSContainerSerializer::IsArray<CleanT>())
        {
            return FDSContainerSerializer::DeserializeArray(
                Bytes,
                OutValue,
                [ObjectRegistry, CurrentDepth](const TArray<uint8>& ElementBytes, auto& Element) -> bool
                {
                    using ElementType = std::decay_t<decltype(Element)>;

                    return FDSValueSerializer::Deserialize<ElementType>(
                        ElementBytes,
                        Element,
                        ObjectRegistry,
                        CurrentDepth + 1
                    );
                }
            );
        }
        else if constexpr (FDSContainerSerializer::IsSet<CleanT>())
        {
            return FDSContainerSerializer::DeserializeSet(
                Bytes,
                OutValue,
                [ObjectRegistry, CurrentDepth](const TArray<uint8>& ElementBytes, auto& Element) -> bool
                {
                    using ElementType = std::decay_t<decltype(Element)>;

                    return FDSValueSerializer::Deserialize<ElementType>(
                        ElementBytes,
                        Element,
                        ObjectRegistry,
                        CurrentDepth + 1
                    );
                }
            );
        }
        else if constexpr (FDSContainerSerializer::IsMap<CleanT>())
        {
            return FDSContainerSerializer::DeserializeMap(
                Bytes,
                OutValue,
                [ObjectRegistry, CurrentDepth](const TArray<uint8>& KeyBytes, auto& Key) -> bool
                {
                    using KeyType = std::decay_t<decltype(Key)>;

                    return FDSValueSerializer::Deserialize<KeyType>(
                        KeyBytes,
                        Key,
                        ObjectRegistry,
                        CurrentDepth + 1
                    );
                },
                [ObjectRegistry, CurrentDepth](const TArray<uint8>& ValueBytes, auto& MapValue) -> bool
                {
                    using ValueType = std::decay_t<decltype(MapValue)>;

                    return FDSValueSerializer::Deserialize<ValueType>(
                        ValueBytes,
                        MapValue,
                        ObjectRegistry,
                        CurrentDepth + 1
                    );
                }
            );
        }

        // =====================================================
        // Unreal string/name/text
        // =====================================================

        else if constexpr (std::is_same_v<CleanT, FString>)
        {
            return FDSStringSpecialCase::Deserialize(
                Bytes,
                OutValue
            );
        }
        else if constexpr (std::is_same_v<CleanT, FName>)
        {
            return FDSNameSpecialCase::Deserialize(
                Bytes,
                OutValue
            );
        }
        else if constexpr (std::is_same_v<CleanT, FText>)
        {
            return FDSFTextSpecialCase::Deserialize(
                Bytes,
                OutValue
            );
        }

        // =====================================================
        // Math types
        // =====================================================

        else if constexpr (FDSMathTypeSerializer::IsSupported<CleanT>())
        {
            return FDSMathTypeSerializer::Deserialize<CleanT>(
                Bytes,
                OutValue
            );
        }

        // =====================================================
        // Reflected USTRUCT
        //
        // Must come before raw binary.
        // =====================================================

        else if constexpr (TDSStructHasStaticStruct<CleanT>::Value)
        {
            return FDSStructSerializer::Deserialize<CleanT>(
                Bytes,
                OutValue
            );
        }

        // =====================================================
        // Custom serializers
        // =====================================================

        else if constexpr (TDSCustomSerializer<CleanT>::bSupported)
        {
            return TDSCustomSerializer<CleanT>::Deserialize(
                Bytes,
                OutValue
            );
        }

        // =====================================================
        // Raw binary fallback
        // =====================================================

        else if constexpr (IsRawBinarySupported<CleanT>())
        {
            if (Bytes.Num() != sizeof(CleanT))
            {
                return false;
            }

            FMemory::Memcpy(&OutValue, Bytes.GetData(), sizeof(CleanT));
            return true;
        }

        // =====================================================
        // Unsupported
        // =====================================================

        else
        {
            UE_LOG(
                LogTemp,
                Error,
                TEXT("DynamicStorage: Unsupported type for deserialization.")
            );

            return false;
        }
    }
};