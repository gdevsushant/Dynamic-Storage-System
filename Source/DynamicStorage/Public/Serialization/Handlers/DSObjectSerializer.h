#pragma once

#include "CoreMinimal.h"
#include <type_traits>

#include "Storage/DSObjectReference.h"

class UDSObjectRegistry;

// =====================================================
// Type traits
// =====================================================

template<typename T>
struct TDSIsTObjectPtr
{
    static constexpr bool Value = false;
};

template<typename T>
struct TDSIsTObjectPtr<TObjectPtr<T>>
{
    static constexpr bool Value = true;
    using ObjectType = T;
};

template<typename T>
struct TDSIsWeakObjectPtr
{
    static constexpr bool Value = false;
};

template<typename T>
struct TDSIsWeakObjectPtr<TWeakObjectPtr<T>>
{
    static constexpr bool Value = true;
    using ObjectType = T;
};

template<typename T>
struct TDSIsSoftObjectPtr
{
    static constexpr bool Value = false;
};

template<typename T>
struct TDSIsSoftObjectPtr<TSoftObjectPtr<T>>
{
    static constexpr bool Value = true;
    using ObjectType = T;
};

/**
 * Handles UObject-style references.
 *
 * Supported:
 * - UObject*
 * - AActor*
 * - UActorComponent*
 * - TObjectPtr<T>
 * - TWeakObjectPtr<T>
 * - TSoftObjectPtr<T>
 *
 * This stores references, not full object state.
 */
class DYNAMICSTORAGE_API FDSObjectSerializer
{
public:

    template<typename T>
    static constexpr bool IsRawObjectPointer()
    {
        return
            std::is_pointer_v<T> &&
            std::is_base_of_v<UObject, std::remove_pointer_t<T>>;
    }

    template<typename T>
    static constexpr bool IsSupported()
    {
        return
            IsRawObjectPointer<T>() ||
            TDSIsTObjectPtr<T>::Value ||
            TDSIsWeakObjectPtr<T>::Value ||
            TDSIsSoftObjectPtr<T>::Value;
    }

    template<typename T>
    static bool Serialize(
        const T& Value,
        TArray<uint8>& OutBytes,
        UDSObjectRegistry* ObjectRegistry
    )
    {
        static_assert(
            IsSupported<T>(),
            "FDSObjectSerializer only supports UObject pointer/reference wrapper types."
        );

        return SerializeImpl(Value, OutBytes, ObjectRegistry);
    }

    template<typename T>
    static bool Deserialize(
        const TArray<uint8>& Bytes,
        T& OutValue,
        UDSObjectRegistry* ObjectRegistry
    )
    {
        static_assert(
            IsSupported<T>(),
            "FDSObjectSerializer only supports UObject pointer/reference wrapper types."
        );

        return DeserializeImpl(Bytes, OutValue, ObjectRegistry);
    }

private:
    
    // =====================================================
// Raw UObject pointer
// =====================================================

template<typename TObjectType>
static bool SerializeImpl(
    TObjectType* ObjectPtr,
    TArray<uint8>& OutBytes,
    UDSObjectRegistry* ObjectRegistry
)
{
    UObject* Object = Cast<UObject>(ObjectPtr);

    FDSObjectReference Reference =
        MakeReferenceFromObject(Object, ObjectRegistry);

    return SerializeReference(Reference, OutBytes);
}

template<typename TObjectType>
static bool DeserializeImpl(
    const TArray<uint8>& Bytes,
    TObjectType*& OutObjectPtr,
    UDSObjectRegistry* ObjectRegistry
)
{
    OutObjectPtr = nullptr;

    FDSObjectReference Reference;

    if (!DeserializeReference(Bytes, Reference))
    {
        return false;
    }

    UObject* ResolvedObject =
        ResolveReference(Reference, ObjectRegistry);

    if (!ResolvedObject)
    {
        return Reference.IsNull();
    }

    OutObjectPtr = Cast<TObjectType>(ResolvedObject);
    return OutObjectPtr != nullptr;
}

// =====================================================
// TObjectPtr
// =====================================================

template<typename TObjectType>
static bool SerializeImpl(
    const TObjectPtr<TObjectType>& ObjectPtr,
    TArray<uint8>& OutBytes,
    UDSObjectRegistry* ObjectRegistry
)
{
    UObject* Object = Cast<UObject>(ObjectPtr.Get());

    FDSObjectReference Reference =
        MakeReferenceFromObject(Object, ObjectRegistry);

    return SerializeReference(Reference, OutBytes);
}

template<typename TObjectType>
static bool DeserializeImpl(
    const TArray<uint8>& Bytes,
    TObjectPtr<TObjectType>& OutObjectPtr,
    UDSObjectRegistry* ObjectRegistry
)
{
    OutObjectPtr = nullptr;

    FDSObjectReference Reference;

    if (!DeserializeReference(Bytes, Reference))
    {
        return false;
    }

    UObject* ResolvedObject =
        ResolveReference(Reference, ObjectRegistry);

    if (!ResolvedObject)
    {
        return Reference.IsNull();
    }

    OutObjectPtr = Cast<TObjectType>(ResolvedObject);
    return OutObjectPtr != nullptr;
}

// =====================================================
// TWeakObjectPtr
// =====================================================

template<typename TObjectType>
static bool SerializeImpl(
    const TWeakObjectPtr<TObjectType>& ObjectPtr,
    TArray<uint8>& OutBytes,
    UDSObjectRegistry* ObjectRegistry
)
{
    UObject* Object = Cast<UObject>(ObjectPtr.Get());

    FDSObjectReference Reference =
        MakeReferenceFromObject(Object, ObjectRegistry);

    return SerializeReference(Reference, OutBytes);
}

template<typename TObjectType>
static bool DeserializeImpl(
    const TArray<uint8>& Bytes,
    TWeakObjectPtr<TObjectType>& OutObjectPtr,
    UDSObjectRegistry* ObjectRegistry
)
{
    OutObjectPtr = nullptr;

    FDSObjectReference Reference;

    if (!DeserializeReference(Bytes, Reference))
    {
        return false;
    }

    UObject* ResolvedObject =
        ResolveReference(Reference, ObjectRegistry);

    if (!ResolvedObject)
    {
        return Reference.IsNull();
    }

    TObjectType* CastedObject = Cast<TObjectType>(ResolvedObject);

    if (!CastedObject)
    {
        return false;
    }

    OutObjectPtr = CastedObject;
    return OutObjectPtr.IsValid();
}

// =====================================================
// TSoftObjectPtr
// =====================================================

template<typename TObjectType>
static bool SerializeImpl(
    const TSoftObjectPtr<TObjectType>& ObjectPtr,
    TArray<uint8>& OutBytes,
    UDSObjectRegistry* ObjectRegistry
)
{
    FDSObjectReference Reference;

    if (ObjectPtr.IsNull())
    {
        Reference.Type = EDSObjectReferenceType::Null;
    }
    else
    {
        Reference.Type = EDSObjectReferenceType::SoftObjectPath;
        Reference.Path = ObjectPtr.ToSoftObjectPath().ToString();
    }

    return SerializeReference(Reference, OutBytes);
}

template<typename TObjectType>
static bool DeserializeImpl(
    const TArray<uint8>& Bytes,
    TSoftObjectPtr<TObjectType>& OutObjectPtr,
    UDSObjectRegistry* ObjectRegistry
)
{
    OutObjectPtr = nullptr;

    FDSObjectReference Reference;

    if (!DeserializeReference(Bytes, Reference))
    {
        return false;
    }

    if (Reference.Type == EDSObjectReferenceType::Null || Reference.Path.IsEmpty())
    {
        OutObjectPtr = TSoftObjectPtr<TObjectType>();
        return true;
    }

    FSoftObjectPath SoftPath(Reference.Path);
    OutObjectPtr = TSoftObjectPtr<TObjectType>(SoftPath);

    return true;
}

    template<typename T>
    static void ResetValue(T& Value)
    {
        if constexpr (IsRawObjectPointer<T>())
        {
            Value = nullptr;
        }
        else
        {
            Value = T();
        }
    }

    static FDSObjectReference MakeReferenceFromObject(
        UObject* Object,
        UDSObjectRegistry* ObjectRegistry
    );

    static UObject* ResolveReference(
        const FDSObjectReference& Reference,
        UDSObjectRegistry* ObjectRegistry
    );

    static bool SerializeReference(
        const FDSObjectReference& Reference,
        TArray<uint8>& OutBytes
    );

    static bool DeserializeReference(
        const TArray<uint8>& Bytes,
        FDSObjectReference& OutReference
    );
};