// Copyright (c) Sushant 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <type_traits>

/**
 * Container type traits
 */

// =====================================================
// 🔹 ARRAY TRAIT
// =====================================================

template<typename T>
struct TDSIsArray
{
    static constexpr bool Value = false;
};

template<typename ElementType, typename AllocatorType>
struct TDSIsArray<TArray<ElementType, AllocatorType>>
{
    static constexpr bool Value = true;
    using InnerType = ElementType;
};


// =====================================================
// 🔹 SET TRAIT
// =====================================================

template<typename T>
struct TDSIsSet
{
    static constexpr bool Value = false;
};

template<typename ElementType, typename KeyFuncs, typename Allocator>
struct TDSIsSet<TSet<ElementType, KeyFuncs, Allocator>>
{
    static constexpr bool Value = true;
    using ElementTypeValue = ElementType;
};

template<typename T>
struct TDSIsMap
{
    static constexpr bool Value = false;
};

template<typename KeyType, typename ValueType, typename SetAllocator, typename KeyFuncs>
struct TDSIsMap<TMap<KeyType, ValueType, SetAllocator, KeyFuncs>>
{
    static constexpr bool Value = true;

    using KeyTypeValue = KeyType;
    using MappedTypeValue = ValueType;
    using ValueTypeValue = ValueType;
};


/**
 * Handles container serialization.
 *
 * Current support:
 * - TArray<T>
 * - TSet<T>
 * - TMap<K, V>
 */
class DYNAMICSTORAGE_API FDSContainerSerializer
{
public:

    template<typename T>
    static constexpr bool IsArray()
    {
        return TDSIsArray<T>::Value;
    }

    template<typename T>
    static constexpr bool IsSet()
    {
        return TDSIsSet<T>::Value;
    }

    // =====================================================
    // 🔹 ARRAY SERIALIZATION
    // =====================================================

    template<typename ArrayType, typename SerializeElementFunc>
    static bool SerializeArray(
        const ArrayType& Array,
        TArray<uint8>& OutBytes,
        SerializeElementFunc&& SerializeElement
    )
    {
        OutBytes.Empty();

        FMemoryWriter Writer(OutBytes);

        int32 Count = Array.Num();
        Writer << Count;

        for (const auto& Element : Array)
        {
            TArray<uint8> ElementBytes;

            if (!SerializeElement(Element, ElementBytes))
            {
                return false;
            }

            int32 ElementSize = ElementBytes.Num();
            Writer << ElementSize;

            if (ElementSize > 0)
            {
                Writer.Serialize(
                    ElementBytes.GetData(),
                    ElementSize
                );
            }
        }

        return true;
    }

    template<typename ArrayType, typename DeserializeElementFunc>
    static bool DeserializeArray(
        const TArray<uint8>& Bytes,
        ArrayType& OutArray,
        DeserializeElementFunc&& DeserializeElement
    )
    {
        OutArray.Empty();

        if (Bytes.Num() <= 0)
        {
            return false;
        }

        FMemoryReader Reader(Bytes);

        int32 Count = 0;
        Reader << Count;

        if (Count < 0)
        {
            return false;
        }

        OutArray.Reserve(Count);

        using ElementType = typename TDSIsArray<ArrayType>::InnerType;

        for (int32 Index = 0; Index < Count; ++Index)
        {
            int32 ElementSize = 0;
            Reader << ElementSize;

            if (ElementSize < 0)
            {
                return false;
            }

            TArray<uint8> ElementBytes;
            ElementBytes.SetNumUninitialized(ElementSize);

            if (ElementSize > 0)
            {
                Reader.Serialize(
                    ElementBytes.GetData(),
                    ElementSize
                );
            }

            ElementType Element{};

            if (!DeserializeElement(ElementBytes, Element))
            {
                return false;
            }

            OutArray.Add(MoveTemp(Element));
        }

        return true;
    }

    // =====================================================
    // 🔹 SET SERIALIZATION
    // =====================================================

    template<typename SetType, typename SerializeElementFunc>
    static bool SerializeSet(
        const SetType& Set,
        TArray<uint8>& OutBytes,
        SerializeElementFunc&& SerializeElement
    )
    {
        OutBytes.Empty();

        FMemoryWriter Writer(OutBytes);

        int32 Count = Set.Num();
        Writer << Count;

        for (const auto& Element : Set)
        {
            TArray<uint8> ElementBytes;

            if (!SerializeElement(Element, ElementBytes))
            {
                return false;
            }

            int32 ElementSize = ElementBytes.Num();
            Writer << ElementSize;

            if (ElementSize > 0)
            {
                Writer.Serialize(
                    ElementBytes.GetData(),
                    ElementSize
                );
            }
        }

        return true;
    }

    template<typename SetType, typename DeserializeElementFunc>
    static bool DeserializeSet(
        const TArray<uint8>& Bytes,
        SetType& OutSet,
        DeserializeElementFunc&& DeserializeElement
    )
    {
        OutSet.Empty();

        if (Bytes.Num() <= 0)
        {
            return false;
        }

        FMemoryReader Reader(Bytes);

        int32 Count = 0;
        Reader << Count;

        if (Count < 0)
        {
            return false;
        }

        using ElementType = typename TDSIsSet<SetType>::ElementTypeValue;

        for (int32 Index = 0; Index < Count; ++Index)
        {
            int32 ElementSize = 0;
            Reader << ElementSize;

            if (ElementSize < 0)
            {
                return false;
            }

            TArray<uint8> ElementBytes;
            ElementBytes.SetNumUninitialized(ElementSize);

            if (ElementSize > 0)
            {
                Reader.Serialize(
                    ElementBytes.GetData(),
                    ElementSize
                );
            }

            ElementType Element{};

            if (!DeserializeElement(ElementBytes, Element))
            {
                return false;
            }

            OutSet.Add(MoveTemp(Element));
        }

        return true;
    }

    // =====================================================
// 🔹 MAP TRAIT
// =====================================================

    template<typename T>
    struct TDSIsMap
    {
        static constexpr bool Value = false;
    };

    template<typename KeyType, typename ValueType, typename SetAllocator, typename KeyFuncs>
    struct TDSIsMap<TMap<KeyType, ValueType, SetAllocator, KeyFuncs>>
    {
        static constexpr bool Value = true;
        using KeyTypeValue = KeyType;
        using MappedTypeValue = ValueType;
    };

        template<typename T>
    static constexpr bool IsMap()
    {
        return TDSIsMap<T>::Value;
    }

    template<typename MapType, typename SerializeKeyFunc, typename SerializeValueFunc>
    static bool SerializeMap(
        const MapType& Map,
        TArray<uint8>& OutBytes,
        SerializeKeyFunc&& SerializeKey,
        SerializeValueFunc&& SerializeValue
    )
    {
        OutBytes.Empty();

        FMemoryWriter Writer(OutBytes);

        int32 Count = Map.Num();
        Writer << Count;

        for (const auto& Pair : Map)
        {
            // -------------------------
            // Key
            // -------------------------
            TArray<uint8> KeyBytes;

            if (!SerializeKey(Pair.Key, KeyBytes))
            {
                return false;
            }

            int32 KeySize = KeyBytes.Num();
            Writer << KeySize;

            if (KeySize > 0)
            {
                Writer.Serialize(
                    KeyBytes.GetData(),
                    KeySize
                );
            }

            // -------------------------
            // Value
            // -------------------------
            TArray<uint8> ValueBytes;

            if (!SerializeValue(Pair.Value, ValueBytes))
            {
                return false;
            }

            int32 ValueSize = ValueBytes.Num();
            Writer << ValueSize;

            if (ValueSize > 0)
            {
                Writer.Serialize(
                    ValueBytes.GetData(),
                    ValueSize
                );
            }
        }

        return true;
    }

    template<typename MapType, typename DeserializeKeyFunc, typename DeserializeValueFunc>
    static bool DeserializeMap(
        const TArray<uint8>& Bytes,
        MapType& OutMap,
        DeserializeKeyFunc&& DeserializeKey,
        DeserializeValueFunc&& DeserializeValue
    )
    {
        OutMap.Empty();

        if (Bytes.Num() <= 0)
        {
            return false;
        }

        FMemoryReader Reader(Bytes);

        int32 Count = 0;
        Reader << Count;

        if (Count < 0)
        {
            return false;
        }

        using KeyType = typename TDSIsMap<MapType>::KeyTypeValue;
        using ValueType = typename TDSIsMap<MapType>::MappedTypeValue;

        for (int32 Index = 0; Index < Count; ++Index)
        {
            // -------------------------
            // Key
            // -------------------------
            int32 KeySize = 0;
            Reader << KeySize;

            if (KeySize < 0)
            {
                return false;
            }

            TArray<uint8> KeyBytes;
            KeyBytes.SetNumUninitialized(KeySize);

            if (KeySize > 0)
            {
                Reader.Serialize(
                    KeyBytes.GetData(),
                    KeySize
                );
            }

            KeyType Key{};

            if (!DeserializeKey(KeyBytes, Key))
            {
                return false;
            }

            // -------------------------
            // Value
            // -------------------------
            int32 ValueSize = 0;
            Reader << ValueSize;

            if (ValueSize < 0)
            {
                return false;
            }

            TArray<uint8> ValueBytes;
            ValueBytes.SetNumUninitialized(ValueSize);

            if (ValueSize > 0)
            {
                Reader.Serialize(
                    ValueBytes.GetData(),
                    ValueSize
                );
            }

            ValueType Value{};

            if (!DeserializeValue(ValueBytes, Value))
            {
                return false;
            }

            OutMap.Add(MoveTemp(Key), MoveTemp(Value));
        }

        return true;
    }
};