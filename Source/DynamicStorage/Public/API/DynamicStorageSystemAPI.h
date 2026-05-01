#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"

#include "Storage/DSStorageCore.h"
#include "Serialization/Core/DSValueSerializer.h"
#include "Serialization/Core/DSCustomSerializer.h"
#include "Serialization/Handlers/DSContainerSerializer.h"

#include <type_traits>

#if WITH_EDITOR
#include "Blueprint/DSTypeRegistrySettings.h"
#include "Blueprint/DSTypeDescriptor.h"
#endif

#include "DynamicStorageSystemAPI.generated.h"

class UDSStorageCore;
class UDSObjectRegistry;

#if WITH_EDITOR

template<typename T, typename = void>
struct TDSHasStaticStruct
{
    static constexpr bool Value = false;
};

template<typename T>
struct TDSHasStaticStruct<T, std::void_t<decltype(T::StaticStruct())>>
{
    static constexpr bool Value = true;
};

#endif

/**
 * Runtime entry point for Dynamic Storage System.
 *
 * This class is the ONLY access point for both C++ and Blueprint.
 * It owns the storage core and exposes safe runtime operations.
 */
UCLASS()
class DYNAMICSTORAGE_API UDynamicStorageSystemAPI : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    
    // =====================================================
    // Static Access
    // =====================================================
    static UDynamicStorageSystemAPI* Get(UObject* WorldContextObject);

    // =====================================================
    // C++ Storage Operations
    // =====================================================

    template<typename T>
    static void RegisterData(
        UObject* WorldContextObject,
        const FGameplayTag& Tag,
        const T& Data
    )
    {
        if (!Tag.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("DynamicStorage: RegisterData failed. Invalid GameplayTag."));
            ensureMsgf(false, TEXT("DynamicStorage: RegisterData failed. Invalid GameplayTag."));
            return;
        }

        UDynamicStorageSystemAPI* System = Get(WorldContextObject);

        if (!System || !System->StorageCore)
        {
            UE_LOG(LogTemp, Error, TEXT("DynamicStorage: RegisterData failed. Invalid storage system."));
            ensureMsgf(false, TEXT("DynamicStorage: RegisterData failed. Invalid storage system."));
            return;
        }

        if (System->StorageCore->HasValue(Tag))
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("DynamicStorage: RegisterData overwriting existing data for Tag [%s]."),
                *Tag.ToString()
            );
        }

        TArray<uint8> Bytes;

        if (!FDSValueSerializer::Serialize<T>(
            Data,
            Bytes,
            System->ObjectRegistry
        ))
        {
            UE_LOG(
                LogTemp,
                Error,
                TEXT("DynamicStorage: Failed to serialize data for Tag [%s]."),
                *Tag.ToString()
            );

            ensureMsgf(
                false,
                TEXT("DynamicStorage: Failed to serialize data for Tag [%s]."),
                *Tag.ToString()
            );

            return;
        }

        System->StorageCore->SetRawValue(Tag, Bytes);
        System->StorageCore->SetTypeId(Tag, GetTypeId<T>());

    #if WITH_EDITOR
            TryRegisterBlueprintMetadataForCppData<T>(Tag);
    #endif
    }

    template<typename T>
    static T RetrieveData(
        UObject* WorldContextObject,
        const FGameplayTag& Tag
    )
    {
        T Result{};

        if (!Tag.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("DynamicStorage: RetrieveData failed. Invalid GameplayTag."));
            ensureMsgf(false, TEXT("DynamicStorage: RetrieveData failed. Invalid GameplayTag."));
            return Result;
        }

        UDynamicStorageSystemAPI* System = Get(WorldContextObject);

        if (!System || !System->StorageCore)
        {
            UE_LOG(LogTemp, Error, TEXT("DynamicStorage: RetrieveData failed. Invalid storage system."));
            ensureMsgf(false, TEXT("DynamicStorage: RetrieveData failed. Invalid storage system."));
            return Result;
        }

        FName StoredTypeId;
        const FName RequestedTypeId = GetTypeId<T>();

        if (!System->StorageCore->GetTypeId(Tag, StoredTypeId))
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("DynamicStorage: RetrieveData failed. No type metadata found for Tag [%s]."),
                *Tag.ToString()
            );

            return Result;
        }

        if (StoredTypeId != RequestedTypeId)
        {
            UE_LOG(
                LogTemp,
                Error,
                TEXT("DynamicStorage: RetrieveData failed. Type mismatch for Tag [%s]."),
                *Tag.ToString()
            );

            ensureMsgf(
                false,
                TEXT("DynamicStorage: RetrieveData failed. Type mismatch for Tag [%s]."),
                *Tag.ToString()
            );

            return Result;
        }

        TArray<uint8> Bytes;

        if (!System->StorageCore->GetRawValue(Tag, Bytes))
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("DynamicStorage: RetrieveData failed. No raw data found for Tag [%s]."),
                *Tag.ToString()
            );

            return Result;
        }

        if (!FDSValueSerializer::Deserialize<T>(
            Bytes,
            Result,
            System->ObjectRegistry
        ))
        {
            UE_LOG(
                LogTemp,
                Error,
                TEXT("DynamicStorage: Failed to deserialize data for Tag [%s]."),
                *Tag.ToString()
            );

            ensureMsgf(
                false,
                TEXT("DynamicStorage: Failed to deserialize data for Tag [%s]."),
                *Tag.ToString()
            );

            return T{};
        }

        return Result;
    }

    static bool ContainsData(
        UObject* WorldContextObject,
        const FGameplayTag& Tag
    );

    static bool UnregisterData(
        UObject* WorldContextObject,
        const FGameplayTag& Tag
    );

protected:

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

private:

    friend class UDSBlueprintStorageBridge;
    template<typename T>
    static FName GetTypeId()
    {
        return FName(TEXT("Register and retrieval datatype is mismatched..."));
    }

    // =====================================================
    // Core Storage
    // =====================================================

    UPROPERTY()
    TObjectPtr<UDSStorageCore> StorageCore;

    UPROPERTY()
    TObjectPtr<UDSObjectRegistry> ObjectRegistry;
    
    // =====================================================
    // Internal Blueprint / K2 bridge functions
    // =====================================================

    bool RegisterDataByProperty_Internal(
        const FGameplayTag& Tag,
        FProperty* Property,
        const void* ValuePtr
    );

    bool RetrieveDataByProperty_Internal(
        const FGameplayTag& Tag,
        FProperty* Property,
        void* OutValuePtr
    );

    bool RetrieveBlueprintPropertyData_Internal(
        const FGameplayTag& Tag,
        FProperty* Property,
        void* OutDataPtr
    );

    bool RegisterStructData_Internal(
        const FGameplayTag& Tag,
        UScriptStruct* StructType,
        const void* StructData
    );

    bool RegisterBlueprintPropertyData_Internal(
        const FGameplayTag& Tag,
        FProperty* Property,
        const void* DataPtr
    );

#if WITH_EDITOR

    template<typename T>
    static bool BuildBlueprintTypeDescriptor(
        FDSTypeDescriptor& OutDescriptor
    )
    {
        using CleanT = std::remove_cv_t<std::remove_reference_t<T>>;

        OutDescriptor = FDSTypeDescriptor::MakeInvalid();

        // =====================================================
        // Primitive Blueprint-supported types
        // =====================================================

        if constexpr (std::is_same_v<CleanT, bool>)
        {
            OutDescriptor.Kind = EDSTypeKind::Bool;
            OutDescriptor.DisplayName = TEXT("bool");
            return true;
        }
        else if constexpr (std::is_same_v<CleanT, int32>)
        {
            OutDescriptor.Kind = EDSTypeKind::Int;
            OutDescriptor.DisplayName = TEXT("int32");
            return true;
        }
        else if constexpr (std::is_same_v<CleanT, uint8>)
        {
            OutDescriptor.Kind = EDSTypeKind::Enum;
            OutDescriptor.TypeObject = nullptr;
            OutDescriptor.DisplayName = TEXT("uint8");
            return true;
        }
        else if constexpr (std::is_same_v<CleanT, float>)
        {
            OutDescriptor.Kind = EDSTypeKind::Float;
            OutDescriptor.DisplayName = TEXT("float");
            return true;
        }
        else if constexpr (std::is_same_v<CleanT, double>)
        {
            OutDescriptor.Kind = EDSTypeKind::Double;
            OutDescriptor.DisplayName = TEXT("double");
            return true;
        }
        
        else if constexpr (std::is_same_v<CleanT, int64>)
        {
            OutDescriptor.Kind = EDSTypeKind::Int64;
            OutDescriptor.DisplayName = TEXT("int64");
            return true;
        }

        // =====================================================
        // Unreal string/text types
        // =====================================================

        else if constexpr (std::is_same_v<CleanT, FString>)
        {
            OutDescriptor.Kind = EDSTypeKind::String;
            OutDescriptor.DisplayName = TEXT("FString");
            return true;
        }
        else if constexpr (std::is_same_v<CleanT, FName>)
        {
            OutDescriptor.Kind = EDSTypeKind::Name;
            OutDescriptor.DisplayName = TEXT("FName");
            return true;
        }
        else if constexpr (std::is_same_v<CleanT, FText>)
        {
            OutDescriptor.Kind = EDSTypeKind::Text;
            OutDescriptor.DisplayName = TEXT("FText");
            return true;
        }

        // =====================================================
        // Unreal containers
        //
        // Must be before std::is_class_v, because TArray/TSet/TMap
        // are C++ class types.
        // =====================================================

        else if constexpr (FDSContainerSerializer::IsArray<CleanT>())
        {
            using InnerType = typename TDSIsArray<CleanT>::InnerType;

            FDSTypeDescriptor InnerDescriptor;

            if (!BuildBlueprintTypeDescriptor<InnerType>(InnerDescriptor))
            {
                OutDescriptor = FDSTypeDescriptor::MakeCppOnlyUnsupported();
                return false;
            }

            OutDescriptor.Kind = EDSTypeKind::Array;
            OutDescriptor.InnerKindA = InnerDescriptor.Kind;
            OutDescriptor.InnerTypeObjectA = InnerDescriptor.TypeObject;
            OutDescriptor.DisplayName = FString::Printf(
                TEXT("Array<%s>"),
                *InnerDescriptor.DisplayName
            );

            return true;
        }
        else if constexpr (FDSContainerSerializer::IsSet<CleanT>())
        {
            using InnerType = typename TDSIsSet<CleanT>::ElementTypeValue;

            FDSTypeDescriptor InnerDescriptor;

            if (!BuildBlueprintTypeDescriptor<InnerType>(InnerDescriptor))
            {
                OutDescriptor = FDSTypeDescriptor::MakeCppOnlyUnsupported();
                return false;
            }

            OutDescriptor.Kind = EDSTypeKind::Set;
            OutDescriptor.InnerKindA = InnerDescriptor.Kind;
            OutDescriptor.InnerTypeObjectA = InnerDescriptor.TypeObject;
            OutDescriptor.DisplayName = FString::Printf(
                TEXT("Set<%s>"),
                *InnerDescriptor.DisplayName
            );

            return true;
        }
        else if constexpr (FDSContainerSerializer::IsMap<CleanT>())
        {
            using KeyType = typename TDSIsMap<CleanT>::KeyTypeValue;
            using ValueType = typename TDSIsMap<CleanT>::MappedTypeValue;

            FDSTypeDescriptor KeyDescriptor;
            FDSTypeDescriptor ValueDescriptor;

            if (!BuildBlueprintTypeDescriptor<KeyType>(KeyDescriptor))
            {
                OutDescriptor = FDSTypeDescriptor::MakeCppOnlyUnsupported();
                return false;
            }

            if (!BuildBlueprintTypeDescriptor<ValueType>(ValueDescriptor))
            {
                OutDescriptor = FDSTypeDescriptor::MakeCppOnlyUnsupported();
                return false;
            }

            OutDescriptor.Kind = EDSTypeKind::Map;
            OutDescriptor.InnerKindA = KeyDescriptor.Kind;
            OutDescriptor.InnerTypeObjectA = KeyDescriptor.TypeObject;
            OutDescriptor.InnerKindB = ValueDescriptor.Kind;
            OutDescriptor.InnerTypeObjectB = ValueDescriptor.TypeObject;
            OutDescriptor.DisplayName = FString::Printf(
                TEXT("Map<%s,%s>"),
                *KeyDescriptor.DisplayName,
                *ValueDescriptor.DisplayName
            );

            return true;
        }

        // =====================================================
        // UObject / UClass pointers
        // =====================================================

        else if constexpr (std::is_same_v<CleanT, UClass*>)
        {
            OutDescriptor.Kind = EDSTypeKind::Class;
            OutDescriptor.TypeObject = UObject::StaticClass();
            OutDescriptor.DisplayName = TEXT("UClass");
            return true;
        }
        else if constexpr (
            std::is_pointer_v<CleanT> &&
            std::is_base_of_v<UObject, std::remove_pointer_t<CleanT>>
        )
        {
            using ObjectType = std::remove_pointer_t<CleanT>;

            OutDescriptor.Kind = EDSTypeKind::Object;
            OutDescriptor.TypeObject = ObjectType::StaticClass();
            OutDescriptor.DisplayName = ObjectType::StaticClass()->GetName();
            return true;
        }

        // =====================================================
        // UENUM only
        //
        // Non-UENUM C++ enums are valid for C++ storage,
        // but unsupported for Blueprint metadata.
        // =====================================================

        else if constexpr (std::is_enum_v<CleanT>)
        {
            if constexpr (TIsUEnumClass<CleanT>::Value)
            {
                UEnum* EnumObject = StaticEnum<CleanT>();

                if (!EnumObject)
                {
                    OutDescriptor = FDSTypeDescriptor::MakeCppOnlyUnsupported();
                    return false;
                }

                OutDescriptor.Kind = EDSTypeKind::Enum;
                OutDescriptor.TypeObject = EnumObject;
                OutDescriptor.DisplayName = EnumObject->GetName();
                return true;
            }
            else
            {
                OutDescriptor = FDSTypeDescriptor::MakeCppOnlyUnsupported();
                return false;
            }
        }

        // =====================================================
        // Reflected USTRUCT only
        //
        // Plain C++ structs/classes and STL classes are valid
        // for C++ storage only, but unsupported for Blueprint metadata.
        // =====================================================

        else if constexpr (std::is_class_v<CleanT>)
        {
            if constexpr (TDSHasStaticStruct<CleanT>::Value)
            {
                UScriptStruct* StructObject = CleanT::StaticStruct();

                if (!StructObject)
                {
                    OutDescriptor = FDSTypeDescriptor::MakeCppOnlyUnsupported();
                    return false;
                }

                OutDescriptor.Kind = EDSTypeKind::Struct;
                OutDescriptor.TypeObject = StructObject;
                OutDescriptor.DisplayName = StructObject->GetName();
                return true;
            }
            else
            {
                OutDescriptor = FDSTypeDescriptor::MakeCppOnlyUnsupported();
                return false;
            }
        }

        // =====================================================
        // Everything else is C++-only / Blueprint-unsupported
        // =====================================================

        else
        {
            OutDescriptor = FDSTypeDescriptor::MakeCppOnlyUnsupported();
            return false;
        }
    }

    template<typename T>
    static void TryRegisterBlueprintMetadataForCppData(
        const FGameplayTag& Tag
    )
    {
        if (!Tag.IsValid())
        {
            return;
        }

        FDSTypeDescriptor Descriptor;

        if (!BuildBlueprintTypeDescriptor<T>(Descriptor))
        {
            return;
        }

        if (!Descriptor.IsBlueprintSupported())
        {
            return;
        }

        UDSTypeRegistrySettings::RegisterTypeDescriptor(
            Tag,
            Descriptor
        );
    }

#endif
};