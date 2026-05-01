// Copyright (c) Sushant 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DSObjectRegistry.generated.h"

/**
 * Runtime object registry.
 *
 * Used for runtime spawned/transient objects that cannot be reliably
 * resolved from asset paths.
 */
UCLASS()
class DYNAMICSTORAGE_API UDSObjectRegistry : public UObject
{
    GENERATED_BODY()

public:

    /**
     * Registers an object and returns its runtime id.
     */
    FGuid RegisterObject(UObject* Object);

    /**
     * Removes an object from the registry.
     */
    bool UnregisterObject(UObject* Object);

    /**
     * Resolves an object by runtime id.
     */
    UObject* ResolveObject(const FGuid& RuntimeId) const;

    /**
     * Finds an already registered id for an object.
     */
    bool FindRuntimeId(UObject* Object, FGuid& OutRuntimeId) const;

    /**
     * Clears registry.
     */
    void Clear();

    /**
     * Number of registered runtime objects.
     */
    int32 Num() const;

private:

    UPROPERTY()
    TMap<FGuid, TWeakObjectPtr<UObject>> IdToObject;

    TMap<TObjectKey<UObject>, FGuid> ObjectToId;
};