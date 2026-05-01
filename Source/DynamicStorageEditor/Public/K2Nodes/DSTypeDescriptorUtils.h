// Copyright (c) Sushant 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphPin.h"
#include "Blueprint/DSTypeDescriptor.h"

class DYNAMICSTORAGEEDITOR_API FDSTypeDescriptorUtils
{
public:
	static bool FromPinType(
		const FEdGraphPinType& PinType,
		FDSTypeDescriptor& OutDescriptor
	);
	
	static bool ToPinType(
		const 	FDSTypeDescriptor& Descriptor,
		FEdGraphPinType& OutPinType
	);
	
private:
	static bool FromSinglePinType(
		const FEdGraphPinType& PinType,
		FDSTypeDescriptor& OutDescriptor
	);
	
	static bool ToSinglePinType(
		const FDSTypeDescriptor& Descriptor,
		FEdGraphPinType& OutPinType
	);
};
