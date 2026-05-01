#include "Serialization/Special/DSNameSpecialCase.h"

void FDSNameSpecialCase::Serialize(
    const FName& Value,
    TArray<uint8>& OutBytes
)
{
    OutBytes.Empty();

    FString NameAsString = Value.ToString();

    FMemoryWriter Writer(OutBytes);
    Writer << NameAsString;
}

bool FDSNameSpecialCase::Deserialize(
    const TArray<uint8>& Bytes,
    FName& OutValue
)
{
    if (Bytes.Num() <= 0)
    {
        OutValue = NAME_None;
        return false;
    }

    FString NameAsString;

    FMemoryReader Reader(Bytes);
    Reader << NameAsString;

    OutValue = FName(*NameAsString);
    return true;
}