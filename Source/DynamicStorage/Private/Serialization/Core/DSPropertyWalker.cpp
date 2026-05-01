#include "Serialization/Core/DSPropertyWalker.h"
#include "UObject/UnrealType.h"

bool FDSPropertyWalker::SerializeStructProperties(
    UScriptStruct* Struct,
    void* StructMemory,
    FStructuredArchive::FSlot Slot
)
{
    if (!Struct || !StructMemory)
    {
        return false;
    }

    for (TFieldIterator<FProperty> It(Struct, EFieldIteratorFlags::IncludeSuper); It; ++It)
    {
        FProperty* Property = *It;

        if (!Property)
        {
            continue;
        }

        void* PropertyValuePtr =
            Property->ContainerPtrToValuePtr<void>(StructMemory);

        if (!PropertyValuePtr)
        {
            return false;
        }

        Property->SerializeItem(
            Slot,
            PropertyValuePtr,
            nullptr
        );
    }

    return true;
}