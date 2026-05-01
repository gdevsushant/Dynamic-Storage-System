#include "Serialization/Handlers/DSObjectSerializer.h"

#include "Storage/DSObjectRegistry.h"

#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/MemoryReader.h"

FDSObjectReference FDSObjectSerializer::MakeReferenceFromObject(
    UObject* Object,
    UDSObjectRegistry* ObjectRegistry
)
{
    FDSObjectReference Reference;

    if (!Object)
    {
        Reference.Type = EDSObjectReferenceType::Null;
        return Reference;
    }

    // =====================================================
    // Actor Component
    // =====================================================

    if (UActorComponent* Component = Cast<UActorComponent>(Object))
    {
        AActor* OwnerActor = Component->GetOwner();

        if (OwnerActor)
        {
            Reference.Type = EDSObjectReferenceType::ActorComponent;
            Reference.Path = OwnerActor->GetPathName();
            Reference.ComponentName = Component->GetName();
            return Reference;
        }
    }

    // =====================================================
    // Actor
    // =====================================================

    if (AActor* Actor = Cast<AActor>(Object))
    {
        Reference.Type = EDSObjectReferenceType::WorldActor;
        Reference.Path = Actor->GetPathName();
        return Reference;
    }

    // =====================================================
    // Runtime / Transient Object
    // =====================================================

    if (Object->HasAnyFlags(RF_Transient))
    {
        if (ObjectRegistry)
        {
            Reference.Type = EDSObjectReferenceType::RuntimeRegisteredObject;
            Reference.RuntimeId = ObjectRegistry->RegisterObject(Object);
            return Reference;
        }

        Reference.Type = EDSObjectReferenceType::Null;
        return Reference;
    }

    // =====================================================
    // Normal UObject / Asset Path
    // =====================================================

    Reference.Type = EDSObjectReferenceType::ObjectPath;
    Reference.Path = Object->GetPathName();

    return Reference;
}

UObject* FDSObjectSerializer::ResolveReference(
    const FDSObjectReference& Reference,
    UDSObjectRegistry* ObjectRegistry
)
{
    switch (Reference.Type)
    {
    case EDSObjectReferenceType::Null:
        return nullptr;

    case EDSObjectReferenceType::RuntimeRegisteredObject:
    {
        return ObjectRegistry
            ? ObjectRegistry->ResolveObject(Reference.RuntimeId)
            : nullptr;
    }

    case EDSObjectReferenceType::WorldActor:
    case EDSObjectReferenceType::ObjectPath:
    {
        if (Reference.Path.IsEmpty())
        {
            return nullptr;
        }

        UObject* FoundObject = StaticFindObject(
            UObject::StaticClass(),
            nullptr,
            *Reference.Path
        );

        if (!FoundObject)
        {
            FoundObject = StaticLoadObject(
                UObject::StaticClass(),
                nullptr,
                *Reference.Path
            );
        }

        return FoundObject;
    }

    case EDSObjectReferenceType::ActorComponent:
    {
        if (Reference.Path.IsEmpty() || Reference.ComponentName.IsEmpty())
        {
            return nullptr;
        }

        UObject* FoundOwnerObject = StaticFindObject(
            UObject::StaticClass(),
            nullptr,
            *Reference.Path
        );

        AActor* OwnerActor = Cast<AActor>(FoundOwnerObject);

        if (!OwnerActor)
        {
            return nullptr;
        }

        TArray<UActorComponent*> Components;
        OwnerActor->GetComponents(Components);

        for (UActorComponent* Component : Components)
        {
            if (Component && Component->GetName() == Reference.ComponentName)
            {
                return Component;
            }
        }

        return nullptr;
    }

    case EDSObjectReferenceType::SoftObjectPath:
    {
        if (Reference.Path.IsEmpty())
        {
            return nullptr;
        }

        FSoftObjectPath SoftPath(Reference.Path);

        UObject* ResolvedObject = SoftPath.ResolveObject();

        if (!ResolvedObject)
        {
            ResolvedObject = SoftPath.TryLoad();
        }

        return ResolvedObject;
    }

    default:
        return nullptr;
    }
}

bool FDSObjectSerializer::SerializeReference(
    const FDSObjectReference& Reference,
    TArray<uint8>& OutBytes
)
{
    OutBytes.Empty();

    FMemoryWriter Writer(OutBytes);

    uint8 TypeByte = static_cast<uint8>(Reference.Type);

    Writer << TypeByte;

    FString Path = Reference.Path;
    Writer << Path;

    FGuid RuntimeId = Reference.RuntimeId;
    Writer << RuntimeId;

    FString ComponentName = Reference.ComponentName;
    Writer << ComponentName;

    return true;
}

bool FDSObjectSerializer::DeserializeReference(
    const TArray<uint8>& Bytes,
    FDSObjectReference& OutReference
)
{
    if (Bytes.Num() <= 0)
    {
        OutReference = FDSObjectReference::MakeNull();
        return false;
    }

    FMemoryReader Reader(Bytes);

    uint8 TypeByte = 0;
    Reader << TypeByte;

    OutReference.Type = static_cast<EDSObjectReferenceType>(TypeByte);

    Reader << OutReference.Path;
    Reader << OutReference.RuntimeId;
    Reader << OutReference.ComponentName;

    return true;
}