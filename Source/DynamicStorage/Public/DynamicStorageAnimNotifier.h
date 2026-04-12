// Copyright (c) Sushant Chahar. 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "DynamicStorageAnimNotifier.generated.h"

UCLASS(Blueprintable, meta = (DisplayName = "Is Executed(Dynamic Storage System)"))
class DYNAMICSTORAGE_API UDynamicStorageAnimNotifier : public UAnimNotify
{
	GENERATED_BODY()

public:

	UDynamicStorageAnimNotifier();

	// The main function triggered by the animation timeline
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	// This allows you to provide a custom name for the node in the Persona editor
	virtual FString GetNotifyName_Implementation() const override;

#if WITH_EDITOR
	// Helpful for changing the node color in the editor timeline
	virtual FLinearColor GetEditorColor() override { return FLinearColor(0.0f, 0.45f, 1.0f); }
#endif

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Storage System")
	bool ShouldSavedToStorage = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Storage System")
	FName NotifyTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dynamic Storage System")
	FGameplayTag StorageTag;
};