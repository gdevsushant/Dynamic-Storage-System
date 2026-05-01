// Copyright (c) Sushant 2026. All Rights Reserved.

#include "DynamicStorage.h"
#define LOCTEXT_NAMESPACE "FDynamicStorageModule"

void FDynamicStorageModule::StartupModule()
{
}

void FDynamicStorageModule::ShutdownModule()
{
}

static void RegisterBuiltInSerializers()
{	
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDynamicStorageModule, DynamicStorage)
