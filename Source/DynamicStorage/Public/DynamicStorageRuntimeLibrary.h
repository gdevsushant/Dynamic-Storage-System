// Copyright (c) Sushant Chahar. 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/UnrealType.h"
#include "UObject/Class.h"
#include "UObject/StructOnScope.h"
#include "Templates/Models.h"
#include "NativeGameplayTags.h"  
#include <concepts>
#include "DynamicStorageRuntimeLibrary.generated.h"

struct FDynamicValue;

// Trait to detect if a type has a StaticStruct via TBaseStructure (Handles FVector, FRotator, etc...)
template<typename T>
struct TIsUnrealStruct {
	template<typename U> static char Check(decltype(TBaseStructure<U>::Get())*);
	template<typename U> static double Check(...);
	static constexpr bool Value = sizeof(Check<T>(nullptr)) == sizeof(char);
};

UCLASS()
class DYNAMICSTORAGE_API UDynamicStorageRuntimeLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// Blueprint hook, validate the correct datatype, cache in global storage
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Dynamic Value", meta = (CustomStructureParam = "InValue", DisplayName = "Set Dynamic Value"), BlueprintInternalUseOnly)
	static void SetDynamicValue(const FGameplayTag Tag, const int32& InValue);
	DECLARE_FUNCTION(execSetDynamicValue);

	// Blueprint hook, validate the correct datatype, retrieve from global storage
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Dynamic Value", meta = (CustomStructureParam = "OutValue", DisplayName = "Get Dynamic Value"), BlueprintInternalUseOnly)
	static void GetDynamicValue(const FGameplayTag Tag, int32& OutValue);
	DECLARE_FUNCTION(execGetDynamicValue);

	// Internal hook, copy blueprint node input pin datatype & value to output pin
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Custom Nodes", meta = (CustomStructureParam = "InValue,OutValue"), BlueprintInternalUseOnly)
	static void PassThroughValue(const int32& InValue, int32& OutValue);
	DECLARE_FUNCTION(execPassThroughValue);

	// --- C++ ---

	/**C++ hook, validate data, cache in global storage. Handles UStructs (FVector, FRotator, and Custom USTRUCTs) */
	template<typename T>
	static typename TEnableIf<TModels<CStaticStructProvider, T>::Value || TIsUnrealStruct<T>::Value, void>::Type
		SetValue(const FGameplayTag Tag, const T& Value)
	{
		Internal_SetGenericValue(Tag, TBaseStructure<T>::Get(), &Value);
	}

	/** C++ hook, validate data, cache in global storage. Handles Enums safely */
	template<typename T>
	static typename TEnableIf<TIsEnum<T>::Value, void>::Type
		SetValue(const FGameplayTag Tag, const T& Value)
	{
		Internal_SetPrimitiveValue(Tag, &Value, sizeof(T), FName("Enum"));
	}

	/** C++ hook, validate data, cache in global storage. Handles Primitives (int, float, bool) */
	template<typename T>
	static typename TEnableIf<!TModels<CStaticStructProvider, T>::Value && !TIsUnrealStruct<T>::Value && !TIsEnum<T>::Value, void>::Type
		SetValue(const FGameplayTag Tag, const T& Value)
	{
		FName TypeName = NAME_None;

		if constexpr (std::same_as<T, bool>) { TypeName = FName("bool"); }
		else if constexpr (std::same_as<T, int32>) { TypeName = FName("int32"); }
		else if constexpr (std::same_as<T, float>) { TypeName = FName("float"); }
		else if constexpr (std::same_as<T, double>) { TypeName = FName("double"); }
		else if constexpr (std::same_as<T, int64>) { TypeName = FName("int64"); }
		else if constexpr (std::same_as<T, int>) { TypeName = FName("int"); }
		else { TypeName = FName(TNameOf<T>::GetName()); }
		
		/*
		if constexpr (std::same_as<T, bool>)    { TypeName = FName("bool"); }
		else if constexpr (std::same_as<T, int32>) { TypeName = FName("int"); } // BP calls this "int"
		else if constexpr (std::same_as<T, float>) { TypeName = FName("float"); }
		else if constexpr (std::same_as<T, double>){ TypeName = FName("real"); } // UE5 internally uses "real" for doubles
		else if constexpr (std::same_as<T, int64>) { TypeName = FName("int64"); }
		else { TypeName = FName(TNameOf<T>::GetName()); }
		 */

		Internal_SetPrimitiveValue(Tag, &Value, sizeof(T), TypeName);
	}

	/** Getter members */

	// C++ hook, validate data, retrieve from global storage. Handles UStructs (FVector, FRotator, and Custom USTRUCTs)
	template<typename T>
	static typename TEnableIf<!TIsDerivedFrom<T, UObject>::IsDerived, T>::Type GetValue(const FGameplayTag Tag, const T& DefaultValue = T())
	{
		T Result = DefaultValue;
		if constexpr (TModels<CStaticStructProvider, T>::Value || TIsUnrealStruct<T>::Value)
		{
			Internal_GetGenericValue(Tag, TBaseStructure<T>::Get(), &Result);
		}
		else
		{
			Internal_GetPrimitiveValue(Tag, &Result, sizeof(T));
		}
		return Result;
	}

	/** C++ hook, validate data, cache in global storage. Handles UObjects (Actors, Components, etc.) */
	template<typename T>
	static typename TEnableIf<TIsDerivedFrom<T, UObject>::IsDerived, void>::Type
	SetValue(const FGameplayTag Tag, T* Value)
	{
		Internal_SetObjectValue(Tag, Value);
	}

    // C++ hook, validate data, retrieve from global storage. Handles UObjects
	template<typename T>
	static typename TEnableIf<TIsDerivedFrom<T, UObject>::IsDerived, T*>::Type
	GetValue(const FGameplayTag Tag)
	{
		return Cast<T>(Internal_GetObjectValue(Tag));
	}
	
	static void Internal_SetObjectValue(const FGameplayTag Tag, UObject* InObject);
	static UObject* Internal_GetObjectValue(const FGameplayTag Tag);

	// C++ hook, validate data, purge data from global storage
	UFUNCTION(BlueprintCallable, Category = "Dynamic Value", BlueprintInternalUseOnly)
	static void RemoveDynamicValue(const FGameplayTag Tag);

	// C++ hook, validate data, perform a comprehensive purge of all data from global storage
	UFUNCTION(BlueprintCallable, Category = "Dynamic Value", BlueprintInternalUseOnly)
	static void ClearAllDynamicValues();

	// C++ hook, validate data existence in global storage
	UFUNCTION(BlueprintPure, Category = "Dynamic Value", BlueprintInternalUseOnly)
	static bool HasDynamicValue(const FGameplayTag Tag);

	// C++ hook, retrieve global storage
	static TMap<FGameplayTag, FDynamicValue>& GetStorage();

private:
	static void Internal_SetGenericValue(const FGameplayTag Tag, FProperty* Property, const void* ValuePtr); // General setter handler for all possible datatypes of unreal engine
	static void Internal_GetGenericValue(const FGameplayTag Tag, FProperty* Property, void* OutValuePtr); // General getter handler for all possible datatypes of unreal engine
	static void Internal_SetGenericValue(const FGameplayTag Tag, UScriptStruct* Struct, const void* ValuePtr); // Direct setter handler for UStructs, avoid type checking
	static void Internal_GetGenericValue(const FGameplayTag Tag, UScriptStruct* Struct, void* OutValuePtr); // Direct getter handler for UStructs, avoid type checking
	static void Internal_SetPrimitiveValue(const FGameplayTag Tag, const void* ValuePtr, int32 Size, FName TypeName); // Direct setter handler for primitive types, avoid type checking
	static void Internal_GetPrimitiveValue(const FGameplayTag Tag, void* OutValuePtr, int32 Size); // Direct getter handler for primitive types, avoid type checking

	static TMap<FGameplayTag, FDynamicValue> Storage;
};
