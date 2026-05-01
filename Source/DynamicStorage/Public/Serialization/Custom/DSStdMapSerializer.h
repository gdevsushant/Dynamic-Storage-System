// Copyright (c) Sushant 2026. All Rights Reserved.

#pragma once

#include <map>
#include <unordered_map>
#include "CoreMinimal.h"
#include "Serialization/Core/DSCustomSerializer.h"
#include "Serialization/Core/DSValueSerializer.h"

template<typename K, typename V>
struct TDSCustomSerializer<std::map<K, V>>
{
    static constexpr bool bSupported = true;

    static bool Serialize(const std::map<K, V>& Value, TArray<uint8>& OutBytes)
    {
        OutBytes.Empty();

        FMemoryWriter Writer(OutBytes);

        int32 Count = static_cast<int32>(Value.size());
        Writer << Count;

        for (const auto& Pair : Value)
        {
            TArray<uint8> KeyBytes;
            TArray<uint8> ValueBytes;

            if (!FDSValueSerializer::Serialize<K>(Pair.first, KeyBytes))
            {
                return false;
            }

            if (!FDSValueSerializer::Serialize<V>(Pair.second, ValueBytes))
            {
                return false;
            }

            int32 KeySize = KeyBytes.Num();
            Writer << KeySize;

            if (KeySize > 0)
            {
                Writer.Serialize(KeyBytes.GetData(), KeySize);
            }

            int32 ValueSize = ValueBytes.Num();
            Writer << ValueSize;

            if (ValueSize > 0)
            {
                Writer.Serialize(ValueBytes.GetData(), ValueSize);
            }
        }

        return true;
    }

    static bool Deserialize(const TArray<uint8>& Bytes, std::map<K, V>& OutValue)
    {
        OutValue.clear();

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

        for (int32 Index = 0; Index < Count; ++Index)
        {
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
                Reader.Serialize(KeyBytes.GetData(), KeySize);
            }

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
                Reader.Serialize(ValueBytes.GetData(), ValueSize);
            }

            K Key{};
            V MapValue{};

            if (!FDSValueSerializer::Deserialize<K>(KeyBytes, Key))
            {
                return false;
            }

            if (!FDSValueSerializer::Deserialize<V>(ValueBytes, MapValue))
            {
                return false;
            }

            OutValue.emplace(std::move(Key), std::move(MapValue));
        }

        return true;
    }
};

template<typename K, typename V>
struct TDSCustomSerializer<std::unordered_map<K, V>>
{
    static constexpr bool bSupported = true;

    static bool Serialize(const std::unordered_map<K, V>& Value, TArray<uint8>& OutBytes)
    {
        OutBytes.Empty();

        FMemoryWriter Writer(OutBytes);

        int32 Count = static_cast<int32>(Value.size());
        Writer << Count;

        for (const auto& Pair : Value)
        {
            TArray<uint8> KeyBytes;
            TArray<uint8> ValueBytes;

            if (!FDSValueSerializer::Serialize<K>(Pair.first, KeyBytes))
            {
                return false;
            }

            if (!FDSValueSerializer::Serialize<V>(Pair.second, ValueBytes))
            {
                return false;
            }

            int32 KeySize = KeyBytes.Num();
            Writer << KeySize;

            if (KeySize > 0)
            {
                Writer.Serialize(KeyBytes.GetData(), KeySize);
            }

            int32 ValueSize = ValueBytes.Num();
            Writer << ValueSize;

            if (ValueSize > 0)
            {
                Writer.Serialize(ValueBytes.GetData(), ValueSize);
            }
        }

        return true;
    }

    static bool Deserialize(const TArray<uint8>& Bytes, std::unordered_map<K, V>& OutValue)
    {
        OutValue.clear();

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

        OutValue.reserve(static_cast<size_t>(Count));

        for (int32 Index = 0; Index < Count; ++Index)
        {
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
                Reader.Serialize(KeyBytes.GetData(), KeySize);
            }

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
                Reader.Serialize(ValueBytes.GetData(), ValueSize);
            }

            K Key{};
            V MapValue{};

            if (!FDSValueSerializer::Deserialize<K>(KeyBytes, Key))
            {
                return false;
            }

            if (!FDSValueSerializer::Deserialize<V>(ValueBytes, MapValue))
            {
                return false;
            }

            OutValue.emplace(std::move(Key), std::move(MapValue));
        }

        return true;
    }
};