#include "Serialization/Handlers/DSStructSerializer.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

bool FDSStructSerializer::SerializeByStruct(
    UScriptStruct* StructType,
    const void* StructData,
    TArray<uint8>& OutBytes
)
{
    OutBytes.Empty();

    if (!StructType || !StructData)
    {
        return false;
    }

    FMemoryWriter MemoryWriter(OutBytes, true);

    FObjectAndNameAsStringProxyArchive Archive(
        MemoryWriter,
        true
    );

    StructType->SerializeItem(
        Archive,
        const_cast<void*>(StructData),
        nullptr
    );

    return !Archive.IsError();
}

bool FDSStructSerializer::DeserializeByStruct(
    UScriptStruct* StructType,
    const TArray<uint8>& Bytes,
    void* StructData
)
{
    if (!StructType || !StructData || Bytes.Num() <= 0)
    {
        return false;
    }

    FMemoryReader MemoryReader(Bytes, true);

    FObjectAndNameAsStringProxyArchive Archive(
        MemoryReader,
        true
    );

    StructType->SerializeItem(
        Archive,
        StructData,
        nullptr
    );

    return !Archive.IsError();
}