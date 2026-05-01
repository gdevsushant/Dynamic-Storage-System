// Copyright (c) Sushant 2026. All Rights Reserved.

#include "Serialization/Special/DSFTextSpecialCase.h"

void FDSFTextSpecialCase::Serialize(
    const FText& Value,
    TArray<uint8>& OutBytes
)
{
    OutBytes.Empty();

    FMemoryWriter Writer(OutBytes);

    FText Temp = Value;
    Writer << Temp;
}

bool FDSFTextSpecialCase::Deserialize(
    const TArray<uint8>& Bytes,
    FText& OutValue
)
{
    if (Bytes.Num() <= 0)
    {
        OutValue = FText::GetEmpty();
        return false;
    }

    FMemoryReader Reader(Bytes);
    Reader << OutValue;

    return true;
}