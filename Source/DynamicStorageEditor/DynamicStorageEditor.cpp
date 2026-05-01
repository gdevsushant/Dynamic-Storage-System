// Copyright (c) Sushant 2026. All Rights Reserved.

#include "DynamicStorageEditor.h"

#define LOCTEXT_NAMESPACE "FDynamicStorageEditorModule"

void FDynamicStorageEditorModule::StartupModule()
{
    // Register K2 nodes, pin factories, etc. later
}

void FDynamicStorageEditorModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDynamicStorageEditorModule, DynamicStorageEditor)