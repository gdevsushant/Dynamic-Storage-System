#include "Blueprint/DSBlueprintPropertySerializer.h"

#include "Serialization/Core/DSValueSerializer.h"
#include "Serialization/Handlers/DSStructSerializer.h"
#include "UObject/UnrealType.h"

bool FDSBlueprintPropertySerializer::SerializeProperty(
    FProperty* Property,
    const void* DataPtr,
    TArray<uint8>& OutBytes,
    UDSObjectRegistry* ObjectRegistry
)
{
    OutBytes.Empty();

    if (!Property || !DataPtr)
    {
        return false;
    }

    // =====================================================
    // Bool
    // =====================================================

    if (FBoolProperty* BoolProp = CastField<FBoolProperty>(Property))
    {
        const bool Value = BoolProp->GetPropertyValue(DataPtr);
        return FDSValueSerializer::Serialize<bool>(Value, OutBytes, ObjectRegistry);
    }

    // =====================================================
    // Integers
    // =====================================================

    if (FIntProperty* IntProp = CastField<FIntProperty>(Property))
    {
        const int32 Value = IntProp->GetPropertyValue(DataPtr);
        return FDSValueSerializer::Serialize<int32>(Value, OutBytes, ObjectRegistry);
    }
    
    if (FInt64Property* Int64Prop = CastField<FInt64Property>(Property))
    {
        const int64 Value = Int64Prop->GetPropertyValue(DataPtr);
        return FDSValueSerializer::Serialize<int64>(Value, OutBytes, ObjectRegistry);
    }

    if (FByteProperty* ByteProp = CastField<FByteProperty>(Property))
    {
        const uint8 Value = ByteProp->GetPropertyValue(DataPtr);
        return FDSValueSerializer::Serialize<uint8>(Value, OutBytes, ObjectRegistry);
    }

    // =====================================================
    // Floating point
    // =====================================================

    if (FFloatProperty* FloatProp = CastField<FFloatProperty>(Property))
    {
        const float Value = FloatProp->GetPropertyValue(DataPtr);
        return FDSValueSerializer::Serialize<float>(Value, OutBytes, ObjectRegistry);
    }

    if (FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(Property))
    {
        const double Value = DoubleProp->GetPropertyValue(DataPtr);
        return FDSValueSerializer::Serialize<double>(Value, OutBytes, ObjectRegistry);
    }

    // =====================================================
    // String / Name / Text
    // =====================================================

    if (FStrProperty* StringProp = CastField<FStrProperty>(Property))
    {
        const FString Value = StringProp->GetPropertyValue(DataPtr);
        return FDSValueSerializer::Serialize<FString>(Value, OutBytes, ObjectRegistry);
    }

    if (FNameProperty* NameProp = CastField<FNameProperty>(Property))
    {
        const FName Value = NameProp->GetPropertyValue(DataPtr);
        return FDSValueSerializer::Serialize<FName>(Value, OutBytes, ObjectRegistry);
    }

    if (FTextProperty* TextProp = CastField<FTextProperty>(Property))
    {
        const FText Value = TextProp->GetPropertyValue(DataPtr);
        return FDSValueSerializer::Serialize<FText>(Value, OutBytes, ObjectRegistry);
    }

    // =====================================================
    // Enum
    // =====================================================

    if (FEnumProperty* EnumProp = CastField<FEnumProperty>(Property))
    {
        FNumericProperty* UnderlyingProp = EnumProp->GetUnderlyingProperty();

        if (!UnderlyingProp)
        {
            return false;
        }

        int64 RawValue = 0;

        if (UnderlyingProp->IsInteger())
        {
            RawValue = UnderlyingProp->GetSignedIntPropertyValue(DataPtr);
        }

        return FDSValueSerializer::Serialize<int64>(RawValue, OutBytes, ObjectRegistry);
    }

    // =====================================================
    // Object / Class
    // =====================================================

    if (FObjectProperty* ObjectProp = CastField<FObjectProperty>(Property))
    {
        UObject* ObjectValue = ObjectProp->GetObjectPropertyValue(DataPtr);
        return FDSValueSerializer::Serialize<UObject*>(ObjectValue, OutBytes, ObjectRegistry);
    }

    if (FClassProperty* ClassProp = CastField<FClassProperty>(Property))
    {
        UClass* ClassValue = Cast<UClass>(ClassProp->GetObjectPropertyValue(DataPtr));
        return FDSValueSerializer::Serialize<UClass*>(ClassValue, OutBytes, ObjectRegistry);
    }

    // =====================================================
    // Struct
    // =====================================================

    if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
    {
        return FDSStructSerializer::SerializeByStruct(
            StructProp->Struct,
            DataPtr,
            OutBytes
        );
    }

    // =====================================================
    // Containers
    // =====================================================

    if (FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property))
    {
        return SerializeArrayProperty(
            ArrayProp,
            DataPtr,
            OutBytes,
            ObjectRegistry
        );
    }

    if (FSetProperty* SetProp = CastField<FSetProperty>(Property))
    {
        return SerializeSetProperty(
            SetProp,
            DataPtr,
            OutBytes,
            ObjectRegistry
        );
    }

    if (FMapProperty* MapProp = CastField<FMapProperty>(Property))
    {
        return SerializeMapProperty(
            MapProp,
            DataPtr,
            OutBytes,
            ObjectRegistry
        );
    }

    UE_LOG(
        LogTemp,
        Error,
        TEXT("DynamicStorage: Unsupported Blueprint property type [%s]."),
        *Property->GetClass()->GetName()
    );

    return false;
}

bool FDSBlueprintPropertySerializer::SerializeArrayProperty(
    FArrayProperty* ArrayProperty,
    const void* DataPtr,
    TArray<uint8>& OutBytes,
    UDSObjectRegistry* ObjectRegistry
)
{
    OutBytes.Empty();

    if (!ArrayProperty || !ArrayProperty->Inner || !DataPtr)
    {
        return false;
    }

    FScriptArrayHelper Helper(ArrayProperty, DataPtr);

    FMemoryWriter Writer(OutBytes);

    int32 Count = Helper.Num();
    Writer << Count;

    for (int32 Index = 0; Index < Count; ++Index)
    {
        const void* ElementPtr = Helper.GetRawPtr(Index);

        TArray<uint8> ElementBytes;

        if (!SerializeProperty(
            ArrayProperty->Inner,
            ElementPtr,
            ElementBytes,
            ObjectRegistry
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

bool FDSBlueprintPropertySerializer::SerializeSetProperty(
    FSetProperty* SetProperty,
    const void* DataPtr,
    TArray<uint8>& OutBytes,
    UDSObjectRegistry* ObjectRegistry
)
{
    OutBytes.Empty();

    if (!SetProperty || !SetProperty->ElementProp || !DataPtr)
    {
        return false;
    }

    FScriptSetHelper Helper(SetProperty, DataPtr);

    FMemoryWriter Writer(OutBytes);

    int32 Count = Helper.Num();
    Writer << Count;

    for (int32 Index = 0; Index < Helper.GetMaxIndex(); ++Index)
    {
        if (!Helper.IsValidIndex(Index))
        {
            continue;
        }

        const void* ElementPtr = Helper.GetElementPtr(Index);

        TArray<uint8> ElementBytes;

        if (!SerializeProperty(
            SetProperty->ElementProp,
            ElementPtr,
            ElementBytes,
            ObjectRegistry
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

bool FDSBlueprintPropertySerializer::SerializeMapProperty(
    FMapProperty* MapProperty,
    const void* DataPtr,
    TArray<uint8>& OutBytes,
    UDSObjectRegistry* ObjectRegistry
)
{
    OutBytes.Empty();

    if (!MapProperty || !MapProperty->KeyProp || !MapProperty->ValueProp || !DataPtr)
    {
        return false;
    }

    FScriptMapHelper Helper(MapProperty, DataPtr);

    FMemoryWriter Writer(OutBytes);

    int32 Count = Helper.Num();
    Writer << Count;

    for (int32 Index = 0; Index < Helper.GetMaxIndex(); ++Index)
    {
        if (!Helper.IsValidIndex(Index))
        {
            continue;
        }

        const void* KeyPtr = Helper.GetKeyPtr(Index);
        const void* ValuePtr = Helper.GetValuePtr(Index);

        TArray<uint8> KeyBytes;
        TArray<uint8> ValueBytes;

        if (!SerializeProperty(
            MapProperty->KeyProp,
            KeyPtr,
            KeyBytes,
            ObjectRegistry
        ))
        {
            return false;
        }

        if (!SerializeProperty(
            MapProperty->ValueProp,
            ValuePtr,
            ValueBytes,
            ObjectRegistry
        ))
        {
            return false;
        }

        int32 KeySize = KeyBytes.Num();
        Writer << KeySize;

        if (KeySize > 0)
        {
            Writer.Serialize(KeyBytes.GetData(), KeySize);
        }

        int32 ValueSize = ValueBytes.Num();
        Writer << ValueSize;

        if (ValueSize > 0)
        {
            Writer.Serialize(ValueBytes.GetData(), ValueSize);
        }
    }

    return true;
}

bool FDSBlueprintPropertySerializer::DeserializeProperty(
    FProperty* Property,
    void* DataPtr,
    const TArray<uint8>& Bytes,
    UDSObjectRegistry* ObjectRegistry
)
{
    if (!Property || !DataPtr)
    {
        return false;
    }

    if (FBoolProperty* BoolProp = CastField<FBoolProperty>(Property))
    {
        bool Value = false;

        if (!FDSValueSerializer::Deserialize<bool>(Bytes, Value, ObjectRegistry))
        {
            return false;
        }

        BoolProp->SetPropertyValue(DataPtr, Value);
        return true;
    }

    if (FIntProperty* IntProp = CastField<FIntProperty>(Property))
    {
        int32 Value = 0;

        if (!FDSValueSerializer::Deserialize<int32>(Bytes, Value, ObjectRegistry))
        {
            return false;
        }

        IntProp->SetPropertyValue(DataPtr, Value);
        return true;
    }
    
    if (FInt64Property* Int64Prop = CastField<FInt64Property>(Property))
    {
        int64 Value = 0;

        if (!FDSValueSerializer::Deserialize<int64>(Bytes, Value, ObjectRegistry))
        {
            return false;
        }

        Int64Prop->SetPropertyValue(DataPtr, Value);
        return true;
    }

    if (FByteProperty* ByteProp = CastField<FByteProperty>(Property))
    {
        uint8 Value = 0;

        if (!FDSValueSerializer::Deserialize<uint8>(Bytes, Value, ObjectRegistry))
        {
            return false;
        }

        ByteProp->SetPropertyValue(DataPtr, Value);
        return true;
    }

    if (FFloatProperty* FloatProp = CastField<FFloatProperty>(Property))
    {
        float Value = 0.f;

        if (!FDSValueSerializer::Deserialize<float>(Bytes, Value, ObjectRegistry))
        {
            return false;
        }

        FloatProp->SetPropertyValue(DataPtr, Value);
        return true;
    }

    if (FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(Property))
    {
        double Value = 0.0;

        if (!FDSValueSerializer::Deserialize<double>(Bytes, Value, ObjectRegistry))
        {
            return false;
        }

        DoubleProp->SetPropertyValue(DataPtr, Value);
        return true;
    }

    if (FStrProperty* StringProp = CastField<FStrProperty>(Property))
    {
        FString Value;

        if (!FDSValueSerializer::Deserialize<FString>(Bytes, Value, ObjectRegistry))
        {
            return false;
        }

        StringProp->SetPropertyValue(DataPtr, Value);
        return true;
    }

    if (FNameProperty* NameProp = CastField<FNameProperty>(Property))
    {
        FName Value;

        if (!FDSValueSerializer::Deserialize<FName>(Bytes, Value, ObjectRegistry))
        {
            return false;
        }

        NameProp->SetPropertyValue(DataPtr, Value);
        return true;
    }

    if (FTextProperty* TextProp = CastField<FTextProperty>(Property))
    {
        FText Value;

        if (!FDSValueSerializer::Deserialize<FText>(Bytes, Value, ObjectRegistry))
        {
            return false;
        }

        TextProp->SetPropertyValue(DataPtr, Value);
        return true;
    }

    if (FEnumProperty* EnumProp = CastField<FEnumProperty>(Property))
    {
        int64 RawValue = 0;

        if (!FDSValueSerializer::Deserialize<int64>(Bytes, RawValue, ObjectRegistry))
        {
            uint8 ByteValue = 0;

            if (!FDSValueSerializer::Deserialize<uint8>(Bytes, ByteValue, ObjectRegistry))
            {
                return false;
            }

            RawValue = ByteValue;
        }

        FNumericProperty* UnderlyingProp = EnumProp->GetUnderlyingProperty();

        if (!UnderlyingProp)
        {
            return false;
        }

        UnderlyingProp->SetIntPropertyValue(DataPtr, RawValue);
        return true;
    }

    if (FObjectProperty* ObjectProp = CastField<FObjectProperty>(Property))
    {
        UObject* Value = nullptr;

        if (!FDSValueSerializer::Deserialize<UObject*>(Bytes, Value, ObjectRegistry))
        {
            return false;
        }

        ObjectProp->SetObjectPropertyValue(DataPtr, Value);
        return true;
    }

    if (FClassProperty* ClassProp = CastField<FClassProperty>(Property))
    {
        UClass* Value = nullptr;

        if (!FDSValueSerializer::Deserialize<UClass*>(Bytes, Value, ObjectRegistry))
        {
            return false;
        }

        ClassProp->SetObjectPropertyValue(DataPtr, Value);
        return true;
    }

    if (FStructProperty* StructProp = CastField<FStructProperty>(Property))
    {

        // UE_LOG(
        //         LogTemp,
        //         Warning,
        //         TEXT("DynamicStorage: Deserializing Struct Property. Struct=[%s], Bytes=%d"),
        //         StructProp->Struct ? *StructProp->Struct->GetName() : TEXT("None"),
        //         Bytes.Num()
        //     );

        //     const bool bResult = FDSStructSerializer::DeserializeByStruct(
        //         StructProp->Struct,
        //         Bytes,
        //         DataPtr
        //     );

        //     UE_LOG(
        //         LogTemp,
        //         Warning,
        //         TEXT("DynamicStorage: Struct Deserialize Result=[%s]"),
        //         bResult ? TEXT("True") : TEXT("False")
        // );

        // return bResult;

        return FDSStructSerializer::DeserializeByStruct(
            StructProp->Struct,
            Bytes,
            DataPtr
        );
    }

    if (FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property))
    {
        return DeserializeArrayProperty(
            ArrayProp,
            DataPtr,
            Bytes,
            ObjectRegistry
        );
    }

    if (FSetProperty* SetProp = CastField<FSetProperty>(Property))
    {
        return DeserializeSetProperty(
            SetProp,
            DataPtr,
            Bytes,
            ObjectRegistry
        );
    }

    if (FMapProperty* MapProp = CastField<FMapProperty>(Property))
    {
        return DeserializeMapProperty(
            MapProp,
            DataPtr,
            Bytes,
            ObjectRegistry
        );
    }

    UE_LOG(
        LogTemp,
        Error,
        TEXT("DynamicStorage: Unsupported Blueprint property deserialize type [%s]."),
        *Property->GetClass()->GetName()
    );

    return false;
}

bool FDSBlueprintPropertySerializer::DeserializeArrayProperty(
    FArrayProperty* ArrayProperty,
    void* DataPtr,
    const TArray<uint8>& Bytes,
    UDSObjectRegistry* ObjectRegistry
)
{
    if (!ArrayProperty || !ArrayProperty->Inner || !DataPtr || Bytes.Num() <= 0)
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

    FScriptArrayHelper Helper(ArrayProperty, DataPtr);
    Helper.EmptyAndAddValues(Count);

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

        void* ElementPtr = Helper.GetRawPtr(Index);

        if (!DeserializeProperty(
            ArrayProperty->Inner,
            ElementPtr,
            ElementBytes,
            ObjectRegistry
        ))
        {
            return false;
        }
    }

    return true;
}

bool FDSBlueprintPropertySerializer::DeserializeSetProperty(
    FSetProperty* SetProperty,
    void* DataPtr,
    const TArray<uint8>& Bytes,
    UDSObjectRegistry* ObjectRegistry
)
{
    if (!SetProperty || !SetProperty->ElementProp || !DataPtr || Bytes.Num() <= 0)
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

    FScriptSetHelper Helper(SetProperty, DataPtr);
    Helper.EmptyElements(Count);

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

        const int32 NewIndex =
            Helper.AddDefaultValue_Invalid_NeedsRehash();

        void* ElementPtr = Helper.GetElementPtr(NewIndex);

        if (!DeserializeProperty(
            SetProperty->ElementProp,
            ElementPtr,
            ElementBytes,
            ObjectRegistry
        ))
        {
            return false;
        }
    }

    Helper.Rehash();

    return true;
}

bool FDSBlueprintPropertySerializer::DeserializeMapProperty(
    FMapProperty* MapProperty,
    void* DataPtr,
    const TArray<uint8>& Bytes,
    UDSObjectRegistry* ObjectRegistry
)
{
    if (
        !MapProperty ||
        !MapProperty->KeyProp ||
        !MapProperty->ValueProp ||
        !DataPtr ||
        Bytes.Num() <= 0
    )
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

    FScriptMapHelper Helper(MapProperty, DataPtr);
    Helper.EmptyValues(Count);

    for (int32 Index = 0; Index < Count; ++Index)
    {
        int32 KeySize = 0;
        Reader << KeySize;

        if (KeySize < 0)
        {
            return false;
        }

        TArray<uint8> KeyBytes;
        KeyBytes.SetNumUninitialized(KeySize);

        if (KeySize > 0)
        {
            Reader.Serialize(KeyBytes.GetData(), KeySize);
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

        const int32 NewIndex =
            Helper.AddDefaultValue_Invalid_NeedsRehash();

        void* KeyPtr = Helper.GetKeyPtr(NewIndex);
        void* ValuePtr = Helper.GetValuePtr(NewIndex);

        if (!DeserializeProperty(
            MapProperty->KeyProp,
            KeyPtr,
            KeyBytes,
            ObjectRegistry
        ))
        {
            return false;
        }

        if (!DeserializeProperty(
            MapProperty->ValueProp,
            ValuePtr,
            ValueBytes,
            ObjectRegistry
        ))
        {
            return false;
        }
    }

    Helper.Rehash();

    return true;
}