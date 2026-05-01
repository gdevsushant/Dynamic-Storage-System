// Copyright (c) Sushant 2026. All Rights Reserved.

#pragma once

#include <utility>

#include "CoreMinimal.h"
#include "Serialization/Core/DSCustomSerializer.h"
#include "Serialization/Core/DSValueSerializer.h"

template<typename A, typename B>
struct TDSCustomSerializer<std::pair<A, B>>
{
    static constexpr bool bSupported = true;

    static bool Serialize(const std::pair<A, B>& Value, TArray<uint8>& OutBytes)
    {
        OutBytes.Empty();

        FMemoryWriter Writer(OutBytes);

        TArray<uint8> FirstBytes;
        TArray<uint8> SecondBytes;

        if (!FDSValueSerializer::Serialize<A>(Value.first, FirstBytes))
        {
            return false;
        }

        if (!FDSValueSerializer::Serialize<B>(Value.second, SecondBytes))
        {
            return false;
        }

        int32 FirstSize = FirstBytes.Num();
        int32 SecondSize = SecondBytes.Num();

        Writer << FirstSize;
        if (FirstSize > 0)
        {
            Writer.Serialize(FirstBytes.GetData(), FirstSize);
        }

        Writer << SecondSize;
        if (SecondSize > 0)
        {
            Writer.Serialize(SecondBytes.GetData(), SecondSize);
        }

        return true;
    }

    static bool Deserialize(const TArray<uint8>& Bytes, std::pair<A, B>& OutValue)
    {
        if (Bytes.Num() <= 0)
        {
            return false;
        }

        FMemoryReader Reader(Bytes);

        int32 FirstSize = 0;
        Reader << FirstSize;

        if (FirstSize < 0)
        {
            return false;
        }

        TArray<uint8> FirstBytes;
        FirstBytes.SetNumUninitialized(FirstSize);

        if (FirstSize > 0)
        {
            Reader.Serialize(FirstBytes.GetData(), FirstSize);
        }

        int32 SecondSize = 0;
        Reader << SecondSize;

        if (SecondSize < 0)
        {
            return false;
        }

        TArray<uint8> SecondBytes;
        SecondBytes.SetNumUninitialized(SecondSize);

        if (SecondSize > 0)
        {
            Reader.Serialize(SecondBytes.GetData(), SecondSize);
        }

        A First{};
        B Second{};

        if (!FDSValueSerializer::Deserialize<A>(FirstBytes, First))
        {
            return false;
        }

        if (!FDSValueSerializer::Deserialize<B>(SecondBytes, Second))
        {
            return false;
        }

        OutValue = std::make_pair(std::move(First), std::move(Second));
        return true;
    }
};