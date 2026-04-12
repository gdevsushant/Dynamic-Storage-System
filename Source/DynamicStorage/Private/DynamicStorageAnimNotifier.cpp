// Copyright (c) Sushant Chahar. 2026. All Rights Reserved.

#include "DynamicStorageAnimNotifier.h"
#include "GameplayTagContainer.h"
#include "DynamicStorageProjectSetting.h"
#include "DynamicStorageEditorDynamicDataStructure.h"
#include "DynamicStorageRuntimeLibrary.h"

UDynamicStorageAnimNotifier::UDynamicStorageAnimNotifier()
{

}

void UDynamicStorageAnimNotifier::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (!StorageTag.IsValid()) { UE_LOG(LogTemp, Log, TEXT("Tag is invalid in notifier in received_notify()")); return; }

	UDynamicStorageProjectSetting* Setting = GetMutableDefault<UDynamicStorageProjectSetting>();
	if (!Setting) { UE_LOG(LogTemp, Log, TEXT("Setting is invalid in notifier in received_notify()")); return; }

	FStorageDefinition Def;

	if (ShouldSavedToStorage) {

		Def.PinCategory = FName("name");
		Def.PinSubCategory = NAME_None;
		Def.PinSubObject = nullptr;

		UDynamicStorageRuntimeLibrary::SetValue<FName>(StorageTag, NotifyTag);
	}

	else {

		if (UDynamicStorageRuntimeLibrary::HasDynamicValue(StorageTag)) {

			UDynamicStorageRuntimeLibrary::RemoveDynamicValue(StorageTag);
		}

		Def.PinCategory = FName("bool");
		Def.PinSubCategory = NAME_None;
		Def.PinSubObject = nullptr;

		UDynamicStorageRuntimeLibrary::SetValue<bool>(StorageTag, false);
	}

	Setting->StorageRegistry.Add(StorageTag, Def);
	return;
}

FString UDynamicStorageAnimNotifier::GetNotifyName_Implementation() const
{
	return FString(TEXT("Is Executed(DynamicStorageSystem)"));
}
