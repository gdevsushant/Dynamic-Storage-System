// Copyright (c) Sushant 2026. All Rights Reserved.

#include "DynamicStorageEditor/Public/K2Nodes/DSTypeDescriptorUtils.h"

#include "EdGraphSchema_K2.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"

bool FDSTypeDescriptorUtils::FromPinType(
    const FEdGraphPinType& PinType,
    FDSTypeDescriptor& OutDescriptor
)
{
    OutDescriptor = FDSTypeDescriptor::MakeInvalid();

    // =====================================================
    // Containers
    // =====================================================

    if (PinType.ContainerType == EPinContainerType::Array)
    {
        FDSTypeDescriptor InnerDesc;

        FEdGraphPinType InnerPinType = PinType;
        InnerPinType.ContainerType = EPinContainerType::None;

        if (!FromSinglePinType(InnerPinType, InnerDesc))
        {
            return false;
        }

        OutDescriptor.Kind = EDSTypeKind::Array;
        OutDescriptor.InnerKindA = InnerDesc.Kind;
        OutDescriptor.InnerTypeObjectA = InnerDesc.TypeObject;
        OutDescriptor.DisplayName = FString::Printf(
            TEXT("Array<%s>"),
            *InnerDesc.DisplayName
        );

        return true;
    }

    if (PinType.ContainerType == EPinContainerType::Set)
    {
        FDSTypeDescriptor InnerDesc;

        FEdGraphPinType InnerPinType = PinType;
        InnerPinType.ContainerType = EPinContainerType::None;

        if (!FromSinglePinType(InnerPinType, InnerDesc))
        {
            return false;
        }

        OutDescriptor.Kind = EDSTypeKind::Set;
        OutDescriptor.InnerKindA = InnerDesc.Kind;
        OutDescriptor.InnerTypeObjectA = InnerDesc.TypeObject;
        OutDescriptor.DisplayName = FString::Printf(
            TEXT("Set<%s>"),
            *InnerDesc.DisplayName
        );

        return true;
    }

    if (PinType.ContainerType == EPinContainerType::Map)
    {
        FDSTypeDescriptor KeyDesc;
        FDSTypeDescriptor ValueDesc;

        FEdGraphPinType KeyPinType = PinType;
        KeyPinType.ContainerType = EPinContainerType::None;

        FEdGraphPinType ValuePinType = PinType;
        ValuePinType.ContainerType = EPinContainerType::None;

        KeyPinType.PinCategory = PinType.PinCategory;
        KeyPinType.PinSubCategory = PinType.PinSubCategory;
        KeyPinType.PinSubCategoryObject = PinType.PinSubCategoryObject;

        ValuePinType.PinCategory = PinType.PinValueType.TerminalCategory;
        ValuePinType.PinSubCategory = PinType.PinValueType.TerminalSubCategory;
        ValuePinType.PinSubCategoryObject = PinType.PinValueType.TerminalSubCategoryObject;

        if (!FromSinglePinType(KeyPinType, KeyDesc))
        {
            return false;
        }

        if (!FromSinglePinType(ValuePinType, ValueDesc))
        {
            return false;
        }

        OutDescriptor.Kind = EDSTypeKind::Map;
        OutDescriptor.InnerKindA = KeyDesc.Kind;
        OutDescriptor.InnerTypeObjectA = KeyDesc.TypeObject;
        OutDescriptor.InnerKindB = ValueDesc.Kind;
        OutDescriptor.InnerTypeObjectB = ValueDesc.TypeObject;
        OutDescriptor.DisplayName = FString::Printf(
            TEXT("Map<%s,%s>"),
            *KeyDesc.DisplayName,
            *ValueDesc.DisplayName
        );

        return true;
    }

    return FromSinglePinType(PinType, OutDescriptor);
}

bool FDSTypeDescriptorUtils::FromSinglePinType(
    const FEdGraphPinType& PinType,
    FDSTypeDescriptor& OutDescriptor
)
{
    OutDescriptor = FDSTypeDescriptor::MakeInvalid();

    const FName Category = PinType.PinCategory;

    if (Category == UEdGraphSchema_K2::PC_Boolean)
    {
        OutDescriptor.Kind = EDSTypeKind::Bool;
        OutDescriptor.DisplayName = TEXT("bool");
        return true;
    }

    if (Category == UEdGraphSchema_K2::PC_Int)
    {
        OutDescriptor.Kind = EDSTypeKind::Int;
        OutDescriptor.DisplayName = TEXT("int32");
        return true;
    }
    
    if (Category == UEdGraphSchema_K2::PC_Int64)
    {
        OutDescriptor.Kind = EDSTypeKind::Int64;
        OutDescriptor.DisplayName = TEXT("int64");
        return true;
    }

    if (Category == UEdGraphSchema_K2::PC_Real)
    {
        if (PinType.PinSubCategory == UEdGraphSchema_K2::PC_Double)
        {
            OutDescriptor.Kind = EDSTypeKind::Double;
            OutDescriptor.DisplayName = TEXT("double");
        }
        else
        {
            OutDescriptor.Kind = EDSTypeKind::Float;
            OutDescriptor.DisplayName = TEXT("float");
        }

        return true;
    }

    if (Category == UEdGraphSchema_K2::PC_Float)
    {
        OutDescriptor.Kind = EDSTypeKind::Float;
        OutDescriptor.DisplayName = TEXT("float");
        return true;
    }

    if (Category == UEdGraphSchema_K2::PC_String)
    {
        OutDescriptor.Kind = EDSTypeKind::String;
        OutDescriptor.DisplayName = TEXT("FString");
        return true;
    }

    if (Category == UEdGraphSchema_K2::PC_Name)
    {
        OutDescriptor.Kind = EDSTypeKind::Name;
        OutDescriptor.DisplayName = TEXT("FName");
        return true;
    }

    if (Category == UEdGraphSchema_K2::PC_Text)
    {
        OutDescriptor.Kind = EDSTypeKind::Text;
        OutDescriptor.DisplayName = TEXT("FText");
        return true;
    }

    if (Category == UEdGraphSchema_K2::PC_Struct)
    {
        UObject* TypeObject = PinType.PinSubCategoryObject.Get();

        if (!TypeObject)
        {
            return false;
        }

        OutDescriptor.Kind = EDSTypeKind::Struct;
        OutDescriptor.TypeObject = TypeObject;
        OutDescriptor.DisplayName = TypeObject->GetName();
        return true;
    }

    if (Category == UEdGraphSchema_K2::PC_Byte ||
        Category == UEdGraphSchema_K2::PC_Enum)
    {
        UObject* TypeObject = PinType.PinSubCategoryObject.Get();

        OutDescriptor.Kind = EDSTypeKind::Enum;
        OutDescriptor.TypeObject = TypeObject;
        OutDescriptor.DisplayName = TypeObject ? TypeObject->GetName() : TEXT("Enum");
        return true;
    }

    if (Category == UEdGraphSchema_K2::PC_Object)
    {
        UObject* TypeObject = PinType.PinSubCategoryObject.Get();

        OutDescriptor.Kind = EDSTypeKind::Object;
        OutDescriptor.TypeObject = TypeObject;
        OutDescriptor.DisplayName = TypeObject ? TypeObject->GetName() : TEXT("UObject");
        return true;
    }

    if (Category == UEdGraphSchema_K2::PC_Class)
    {
        UObject* TypeObject = PinType.PinSubCategoryObject.Get();

        OutDescriptor.Kind = EDSTypeKind::Class;
        OutDescriptor.TypeObject = TypeObject;
        OutDescriptor.DisplayName = TypeObject ? TypeObject->GetName() : TEXT("UClass");
        return true;
    }

    OutDescriptor = FDSTypeDescriptor::MakeCppOnlyUnsupported();
    return false;
}

bool FDSTypeDescriptorUtils::ToPinType(
    const FDSTypeDescriptor& Descriptor,
    FEdGraphPinType& OutPinType
)
{
    OutPinType.ResetToDefaults();

    if (!Descriptor.IsBlueprintSupported())
    {
        return false;
    }

    if (Descriptor.Kind == EDSTypeKind::Array)
    {
        FDSTypeDescriptor InnerDesc;
        InnerDesc.Kind = Descriptor.InnerKindA;
        InnerDesc.TypeObject = Descriptor.InnerTypeObjectA;

        if (!ToSinglePinType(InnerDesc, OutPinType))
        {
            return false;
        }

        OutPinType.ContainerType = EPinContainerType::Array;
        return true;
    }

    if (Descriptor.Kind == EDSTypeKind::Set)
    {
        FDSTypeDescriptor InnerDesc;
        InnerDesc.Kind = Descriptor.InnerKindA;
        InnerDesc.TypeObject = Descriptor.InnerTypeObjectA;

        if (!ToSinglePinType(InnerDesc, OutPinType))
        {
            return false;
        }

        OutPinType.ContainerType = EPinContainerType::Set;
        return true;
    }

    if (Descriptor.Kind == EDSTypeKind::Map)
    {
        FDSTypeDescriptor KeyDesc;
        KeyDesc.Kind = Descriptor.InnerKindA;
        KeyDesc.TypeObject = Descriptor.InnerTypeObjectA;

        FDSTypeDescriptor ValueDesc;
        ValueDesc.Kind = Descriptor.InnerKindB;
        ValueDesc.TypeObject = Descriptor.InnerTypeObjectB;

        FEdGraphPinType KeyPinType;
        FEdGraphPinType ValuePinType;

        if (!ToSinglePinType(KeyDesc, KeyPinType))
        {
            return false;
        }

        if (!ToSinglePinType(ValueDesc, ValuePinType))
        {
            return false;
        }

        OutPinType = KeyPinType;
        OutPinType.ContainerType = EPinContainerType::Map;
        OutPinType.PinValueType.TerminalCategory = ValuePinType.PinCategory;
        OutPinType.PinValueType.TerminalSubCategory = ValuePinType.PinSubCategory;
        OutPinType.PinValueType.TerminalSubCategoryObject = ValuePinType.PinSubCategoryObject;

        return true;
    }

    return ToSinglePinType(Descriptor, OutPinType);
}

bool FDSTypeDescriptorUtils::ToSinglePinType(
    const FDSTypeDescriptor& Descriptor,
    FEdGraphPinType& OutPinType
)
{
    OutPinType.ResetToDefaults();

    switch (Descriptor.Kind)
    {
    case EDSTypeKind::Bool:
        OutPinType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
        return true;

    case EDSTypeKind::Int:
        OutPinType.PinCategory = UEdGraphSchema_K2::PC_Int;
        return true;
        
    case EDSTypeKind::Int64:
        OutPinType.PinCategory = UEdGraphSchema_K2::PC_Int64;
        return true;

    case EDSTypeKind::Float:
        OutPinType.PinCategory = UEdGraphSchema_K2::PC_Real;
        OutPinType.PinSubCategory = UEdGraphSchema_K2::PC_Float;
        return true;

    case EDSTypeKind::Double:
        OutPinType.PinCategory = UEdGraphSchema_K2::PC_Real;
        OutPinType.PinSubCategory = UEdGraphSchema_K2::PC_Double;
        return true;

    case EDSTypeKind::String:
        OutPinType.PinCategory = UEdGraphSchema_K2::PC_String;
        return true;

    case EDSTypeKind::Name:
        OutPinType.PinCategory = UEdGraphSchema_K2::PC_Name;
        return true;

    case EDSTypeKind::Text:
        OutPinType.PinCategory = UEdGraphSchema_K2::PC_Text;
        return true;

    case EDSTypeKind::Struct:
        OutPinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
        OutPinType.PinSubCategoryObject = Descriptor.TypeObject;
        return Descriptor.TypeObject != nullptr;

    case EDSTypeKind::Enum:
        OutPinType.PinCategory = UEdGraphSchema_K2::PC_Enum;
        OutPinType.PinSubCategoryObject = Descriptor.TypeObject;
        return true;

    case EDSTypeKind::Object:
        OutPinType.PinCategory = UEdGraphSchema_K2::PC_Object;
        OutPinType.PinSubCategoryObject = Descriptor.TypeObject;
        return true;

    case EDSTypeKind::Class:
        OutPinType.PinCategory = UEdGraphSchema_K2::PC_Class;
        OutPinType.PinSubCategoryObject = Descriptor.TypeObject;
        return true;

    default:
        return false;
    }
}