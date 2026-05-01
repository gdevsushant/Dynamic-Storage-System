#pragma once

#include "CoreMinimal.h"
#include "Serialization/StructuredArchive.h"

/**
 * Walks reflected USTRUCT properties and serializes/deserializes them.
 *
 * This is used only for real reflected USTRUCT types.
 */
class DYNAMICSTORAGE_API FDSPropertyWalker
{
public:

    static bool SerializeStructProperties(
        UScriptStruct* Struct,
        void* StructMemory,
        FStructuredArchive::FSlot Slot
    );
};