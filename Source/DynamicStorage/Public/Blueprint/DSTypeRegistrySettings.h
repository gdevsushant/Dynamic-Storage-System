// Copyright (c) Sushant 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "Blueprint/DSTypeDescriptor.h"
#include "DSTypeRegistrySettings.generated.h"

USTRUCT(BlueprintType)
struct DYNAMICSTORAGE_API FDSRegisteredTypeEntry
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Storage")
    FGameplayTag Tag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Storage")
    FDSTypeDescriptor TypeDescriptor;
};

UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Dynamic Storage Type Registry"))
class DYNAMICSTORAGE_API UDSTypeRegistrySettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, Config, Category="Dynamic Storage")
    TArray<FDSRegisteredTypeEntry> RegisteredTypes;

public:

    static const UDSTypeRegistrySettings* Get();

    static bool FindTypeDescriptor(
        const FGameplayTag& Tag,
        FDSTypeDescriptor& OutDescriptor
    );

#if WITH_EDITOR
    static bool RegisterTypeDescriptor(
        const FGameplayTag& Tag,
        const FDSTypeDescriptor& Descriptor
    );

    static bool UnregisterTypeDescriptor(
        const FGameplayTag& Tag
    );
    
    static void SaveRegistrySettings();
#endif
};