// Copyright (c) Sushant 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphPin.h"
#include "DSTypeDescriptor.generated.h"

UENUM(BlueprintType)
enum class EDSTypeKind : uint8
{
	Invalid,

	Bool,
	Int,
	Int64,
	Float,
	Double,

	String,
	Name,
	Text,

	Struct,
	Enum,
	Object,
	Class,

	Array,
	Set,
	Map,

	CppOnlyUnsupported
};

USTRUCT(BlueprintType)
struct DYNAMICSTORAGE_API FDSTypeDescriptor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Storage")
	EDSTypeKind Kind = EDSTypeKind::Invalid;

	/**
	 * Used for Struct / Enum / Object / Class.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Storage")
	TObjectPtr<UObject> TypeObject = nullptr;

	/**
	 * Used by Array / Set.
	 */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Storage")
	EDSTypeKind InnerKindA = EDSTypeKind::Invalid;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Storage")
	TObjectPtr<UObject> InnerTypeObjectA = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Storage")
	EDSTypeKind InnerKindB = EDSTypeKind::Invalid;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Storage")
	TObjectPtr<UObject> InnerTypeObjectB = nullptr;

	/**
	 * Optional debug/display name.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Storage")
	FString DisplayName;

public:

	bool IsValid() const
	{
		return Kind != EDSTypeKind::Invalid;
	}

	bool IsBlueprintSupported() const
	{
		return Kind != EDSTypeKind::Invalid &&
			   Kind != EDSTypeKind::CppOnlyUnsupported;
	}

	static FDSTypeDescriptor MakeInvalid()
	{
		FDSTypeDescriptor Desc;
		Desc.Kind = EDSTypeKind::Invalid;
		return Desc;
	}

	static FDSTypeDescriptor MakeCppOnlyUnsupported()
	{
		FDSTypeDescriptor Desc;
		Desc.Kind = EDSTypeKind::CppOnlyUnsupported;
		Desc.DisplayName = TEXT("C++ Only Unsupported");
		return Desc;
	}
};