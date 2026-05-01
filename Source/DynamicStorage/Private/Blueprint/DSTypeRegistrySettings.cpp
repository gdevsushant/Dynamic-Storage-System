// Copyright (c) Sushant 2026. All Rights Reserved.

#include "Blueprint/DSTypeRegistrySettings.h"
#include "Misc/ConfigCacheIni.h"

const UDSTypeRegistrySettings* UDSTypeRegistrySettings::Get()
{
    return GetDefault<UDSTypeRegistrySettings>();
}

bool UDSTypeRegistrySettings::FindTypeDescriptor(
    const FGameplayTag& Tag,
    FDSTypeDescriptor& OutDescriptor
)
{
    if (!Tag.IsValid())
    {
        return false;
    }

    const UDSTypeRegistrySettings* Settings = Get();

    if (!Settings)
    {
        return false;
    }

    for (const FDSRegisteredTypeEntry& Entry : Settings->RegisteredTypes)
    {
        if (Entry.Tag == Tag)
        {
            OutDescriptor = Entry.TypeDescriptor;
            return Entry.TypeDescriptor.IsValid();
        }
    }

    return false;
}

#if WITH_EDITOR

bool UDSTypeRegistrySettings::RegisterTypeDescriptor(
    const FGameplayTag& Tag,
    const FDSTypeDescriptor& Descriptor
)
{
    if (!Tag.IsValid() || !Descriptor.IsValid())
    {
        return false;
    }

    UDSTypeRegistrySettings* Settings =
        GetMutableDefault<UDSTypeRegistrySettings>();

    if (!Settings)
    {
        return false;
    }

    Settings->Modify();

    for (FDSRegisteredTypeEntry& Entry : Settings->RegisteredTypes)
    {
        if (Entry.Tag == Tag)
        {
            Entry.TypeDescriptor = Descriptor;
            SaveRegistrySettings();
            return true;
        }
    }

    FDSRegisteredTypeEntry NewEntry;
    NewEntry.Tag = Tag;
    NewEntry.TypeDescriptor = Descriptor;

    Settings->RegisteredTypes.Add(NewEntry);

    SaveRegistrySettings();

    return true;
}

bool UDSTypeRegistrySettings::UnregisterTypeDescriptor(
    const FGameplayTag& Tag
)
{
    if (!Tag.IsValid())
    {
        return false;
    }

    UDSTypeRegistrySettings* Settings =
        GetMutableDefault<UDSTypeRegistrySettings>();

    if (!Settings)
    {
        return false;
    }

    Settings->Modify();

    const int32 RemovedCount =
        Settings->RegisteredTypes.RemoveAll(
            [&Tag](const FDSRegisteredTypeEntry& Entry)
            {
                return Entry.Tag == Tag;
            }
        );

    if (RemovedCount > 0)
    {
        SaveRegistrySettings();
        return true;
    }

    return false;
}

#endif

#if WITH_EDITOR

void UDSTypeRegistrySettings::SaveRegistrySettings()
{
    UDSTypeRegistrySettings* Settings =
        GetMutableDefault<UDSTypeRegistrySettings>();

    if (!Settings)
    {
        return;
    }

    Settings->SaveConfig(
        CPF_Config,
        *Settings->GetDefaultConfigFilename()
    );

    if (GConfig)
    {
        GConfig->Flush(
            false,
            Settings->GetDefaultConfigFilename()
        );
    }
}

#endif