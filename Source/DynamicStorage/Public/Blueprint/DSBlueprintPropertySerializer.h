#pragma once

#include "CoreMinimal.h"

class UDSObjectRegistry;

class DYNAMICSTORAGE_API FDSBlueprintPropertySerializer
{
public:

    static bool SerializeProperty(
        FProperty* Property,
        const void* DataPtr,
        TArray<uint8>& OutBytes,
        UDSObjectRegistry* ObjectRegistry
    );

    static bool DeserializeProperty(
        FProperty* Property,
        void* DataPtr,
        const TArray<uint8>& Bytes,
        UDSObjectRegistry* ObjectRegistry
    );

private:

    static bool SerializeArrayProperty(
        FArrayProperty* ArrayProperty,
        const void* DataPtr,
        TArray<uint8>& OutBytes,
        UDSObjectRegistry* ObjectRegistry
    );

    static bool SerializeSetProperty(
        FSetProperty* SetProperty,
        const void* DataPtr,
        TArray<uint8>& OutBytes,
        UDSObjectRegistry* ObjectRegistry
    );

    static bool SerializeMapProperty(
        FMapProperty* MapProperty,
        const void* DataPtr,
        TArray<uint8>& OutBytes,
        UDSObjectRegistry* ObjectRegistry
    );

    static bool DeserializeArrayProperty(
        FArrayProperty* ArrayProperty,
        void* DataPtr,
        const TArray<uint8>& Bytes,
        UDSObjectRegistry* ObjectRegistry
    );

    static bool DeserializeSetProperty(
        FSetProperty* SetProperty,
        void* DataPtr,
        const TArray<uint8>& Bytes,
        UDSObjectRegistry* ObjectRegistry
    );

    static bool DeserializeMapProperty(
        FMapProperty* MapProperty,
        void* DataPtr,
        const TArray<uint8>& Bytes,
        UDSObjectRegistry* ObjectRegistry
    );
};