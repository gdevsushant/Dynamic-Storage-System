// Copyright (c) Sushant 2026. All Rights Reserved.

#include "Serialization/Special/DSStringSpecialCase.h"

void FDSStringSpecialCase::Serialize(
    const FString& Value,
    TArray<uint8>& OutBytes
)
{
    OutBytes.Empty();

    FMemoryWriter Writer(OutBytes);

    FString Temp = Value;
    Writer << Temp;
}

bool FDSStringSpecialCase::Deserialize(
    const TArray<uint8>& Bytes,
    FString& OutValue
)
{
    if (Bytes.Num() <= 0)
    {
        OutValue.Empty();
        return false;
    }

    FMemoryReader Reader(Bytes);
    Reader << OutValue;

    return true;
}