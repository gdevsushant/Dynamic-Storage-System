#include "Storage/DSObjectRegistry.h"

FGuid UDSObjectRegistry::RegisterObject(UObject* Object)
{
    if (!Object)
    {
        return FGuid();
    }

    const TObjectKey<UObject> ObjectKey(Object);

    if (const FGuid* ExistingId = ObjectToId.Find(ObjectKey))
    {
        return *ExistingId;
    }

    const FGuid NewId = FGuid::NewGuid();

    IdToObject.Add(NewId, Object);
    ObjectToId.Add(ObjectKey, NewId);

    return NewId;
}

bool UDSObjectRegistry::UnregisterObject(UObject* Object)
{
    if (!Object)
    {
        return false;
    }

    const TObjectKey<UObject> ObjectKey(Object);

    FGuid RuntimeId;

    if (!ObjectToId.RemoveAndCopyValue(ObjectKey, RuntimeId))
    {
        return false;
    }

    IdToObject.Remove(RuntimeId);
    return true;
}

UObject* UDSObjectRegistry::ResolveObject(const FGuid& RuntimeId) const
{
    if (!RuntimeId.IsValid())
    {
        return nullptr;
    }

    const TWeakObjectPtr<UObject>* FoundObject = IdToObject.Find(RuntimeId);

    if (!FoundObject)
    {
        return nullptr;
    }

    return FoundObject->Get();
}

bool UDSObjectRegistry::FindRuntimeId(UObject* Object, FGuid& OutRuntimeId) const
{
    if (!Object)
    {
        return false;
    }

    const TObjectKey<UObject> ObjectKey(Object);

    if (const FGuid* FoundId = ObjectToId.Find(ObjectKey))
    {
        OutRuntimeId = *FoundId;
        return true;
    }

    return false;
}

void UDSObjectRegistry::Clear()
{
    IdToObject.Empty();
    ObjectToId.Empty();
}

int32 UDSObjectRegistry::Num() const
{
    return IdToObject.Num();
}