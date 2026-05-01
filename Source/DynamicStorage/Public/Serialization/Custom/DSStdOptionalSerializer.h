#pragma once

#include <optional>

#include "CoreMinimal.h"
#include "Serialization/Core/DSCustomSerializer.h"
#include "Serialization/Core/DSValueSerializer.h"

template<typename T>
struct TDSCustomSerializer<std::optional<T>>
{
    static constexpr bool bSupported = true;

    static bool Serialize(const std::optional<T>& Value, TArray<uint8>& OutBytes)
    {
        OutBytes.Empty();

        FMemoryWriter Writer(OutBytes);

        bool bHasValue = Value.has_value();
        Writer << bHasValue;

        if (!bHasValue)
        {
            return true;
        }

        TArray<uint8> ValueBytes;

        if (!FDSValueSerializer::Serialize<T>(*Value, ValueBytes))
        {
            return false;
        }

        int32 ValueSize = ValueBytes.Num();
        Writer << ValueSize;

        if (ValueSize > 0)
        {
            Writer.Serialize(ValueBytes.GetData(), ValueSize);
        }

        return true;
    }

    static bool Deserialize(const TArray<uint8>& Bytes, std::optional<T>& OutValue)
    {
        OutValue.reset();

        if (Bytes.Num() <= 0)
        {
            return false;
        }

        FMemoryReader Reader(Bytes);

        bool bHasValue = false;
        Reader << bHasValue;

        if (!bHasValue)
        {
            return true;
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

        T Value{};

        if (!FDSValueSerializer::Deserialize<T>(ValueBytes, Value))
        {
            return false;
        }

        OutValue = std::move(Value);
        return true;
    }
};