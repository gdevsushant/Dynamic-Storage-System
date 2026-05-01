#pragma once

#include "CoreMinimal.h"
#include "DSObjectReference.generated.h"

/**
 * Defines how an object reference is stored.
 */
UENUM()
enum class EDSObjectReferenceType : uint8
{
    Null,

    /**
     * Asset or loaded UObject that can be found by path.
     */
    ObjectPath,

    /**
     * UClass / Blueprint class reference.
     */
    ClassPath,

    /**
     * Live actor in the current world.
     */
    WorldActor,

    /**
     * Component owned by an actor.
     */
    ActorComponent,

    /**
     * Runtime/transient object registered in DSObjectRegistry.
     */
    RuntimeRegisteredObject,

    /**
     * Soft object path.
     */
    SoftObjectPath,

    /**
     * Soft class path.
     */
    SoftClassPath
};

/**
 * Serialized representation of an object reference.
 *
 * This struct stores identity information, not the full object data.
 */
USTRUCT()
struct DYNAMICSTORAGE_API FDSObjectReference
{
    GENERATED_BODY()

public:

    UPROPERTY()
    EDSObjectReferenceType Type = EDSObjectReferenceType::Null;

    /**
     * Used for asset/object path, class path, soft paths.
     */
    UPROPERTY()
    FString Path;

    /**
     * Used for runtime registered objects.
     */
    UPROPERTY()
    FGuid RuntimeId;

    /**
     * Used for actor component references.
     */
    UPROPERTY()
    FString ComponentName;

public:

    bool IsNull() const
    {
        return Type == EDSObjectReferenceType::Null;
    }

    static FDSObjectReference MakeNull()
    {
        return FDSObjectReference();
    }
};