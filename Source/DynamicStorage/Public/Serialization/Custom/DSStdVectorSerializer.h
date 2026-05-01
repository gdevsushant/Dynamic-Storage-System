#pragma once

#include <vector>

#include "CoreMinimal.h"
#include "Serialization/Core/DSCustomSerializer.h"
#include "Serialization/Core/DSValueSerializer.h"

template<typename ElementType>
struct TDSCustomSerializer<std::vector<ElementType>>
{
    static constexpr bool bSupported = true;

    static bool Serialize(
        const std::vector<ElementType>& Value,
        TArray<uint8>& OutBytes
    )
    {
        OutBytes.Empty();

        FMemoryWriter Writer(OutBytes);

        int32 Count = static_cast<int32>(Value.size());
        Writer << Count;

        for (const ElementType& Element : Value)
        {
            TArray<uint8> ElementBytes;

            if (!FDSValueSerializer::Serialize<ElementType>(
                Element,
                ElementBytes
            ))
            {
                return false;
            }

            int32 ElementSize = ElementBytes.Num();
            Writer << ElementSize;

            if (ElementSize > 0)
            {
                Writer.Serialize(ElementBytes.GetData(), ElementSize);
            }
        }

        return true;
    }

    static bool Deserialize(
        const TArray<uint8>& Bytes,
        std::vector<ElementType>& OutValue
    )
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
                Reader.Serialize(ElementBytes.GetData(), ElementSize);
            }

            ElementType Element{};

            if (!FDSValueSerializer::Deserialize<ElementType>(
                ElementBytes,
                Element
            ))
            {
                return false;
            }

            OutValue.push_back(std::move(Element));
        }

        return true;
    }
};