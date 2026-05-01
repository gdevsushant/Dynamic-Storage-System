#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "DSBlueprintStorageBridge.generated.h"

class FProperty;

UCLASS()
class DYNAMICSTORAGE_API UDSBlueprintStorageBridge : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    static bool RegisterDataByProperty(
        UObject* WorldContextObject,
        const FGameplayTag& Tag,
        FProperty* Property,
        const void* ValuePtr
    );

    static bool RetrieveDataByProperty(
        UObject* WorldContextObject,
        const FGameplayTag& Tag,
        FProperty* Property,
        void* OutValuePtr
    );
    
    UFUNCTION(BlueprintCallable, Category="Dynamic Storage", meta=(WorldContext="WorldContextObject", BlueprintInternalUseOnly = "true"))
    static bool ContainData(
        UObject* WorldContextObject,
        FGameplayTag Tag
    );

    UFUNCTION(BlueprintCallable, Category="Dynamic Storage", meta=(WorldContext="WorldContextObject", BlueprintInternalUseOnly = "true"))
    static bool UnregisterData(
        UObject* WorldContextObject,
        FGameplayTag Tag
    );
    
    UFUNCTION(BlueprintCallable, Category="Dynamic Storage|Internal", meta=(WorldContext="WorldContextObject", BlueprintInternalUseOnly="true"))
    static void RegisterBoolData(
        UObject* WorldContextObject,
        FGameplayTag Tag,
        bool Data
    );

    UFUNCTION(BlueprintCallable, Category="Dynamic Storage|Internal", meta=(WorldContext="WorldContextObject", BlueprintInternalUseOnly="true"))
    static void RegisterIntData(
        UObject* WorldContextObject,
        FGameplayTag Tag,
        int32 Data
    );
    
    UFUNCTION(BlueprintCallable, Category="Dynamic Storage|Internal", meta=(WorldContext="WorldContextObject", BlueprintInternalUseOnly="true"))
    static void RegisterInt64Data(
        UObject* WorldContextObject,
        FGameplayTag Tag,
        int64 Data
    );

    UFUNCTION(BlueprintCallable, Category="Dynamic Storage|Internal", meta=(WorldContext="WorldContextObject", BlueprintInternalUseOnly="true"))
    static void RegisterFloatData(
        UObject* WorldContextObject,
        FGameplayTag Tag,
        float Data
    );

    UFUNCTION(BlueprintCallable, Category="Dynamic Storage|Internal", meta=(WorldContext="WorldContextObject", BlueprintInternalUseOnly="true"))
    static void RegisterDoubleData(
        UObject* WorldContextObject,
        FGameplayTag Tag,
        double Data
    );

    UFUNCTION(BlueprintCallable, Category="Dynamic Storage|Internal", meta=(WorldContext="WorldContextObject", BlueprintInternalUseOnly="true"))
    static void RegisterStringData(
        UObject* WorldContextObject,
        FGameplayTag Tag,
        FString Data
    );

    UFUNCTION(BlueprintCallable, Category="Dynamic Storage|Internal", meta=(WorldContext="WorldContextObject", BlueprintInternalUseOnly="true"))
    static void RegisterNameData(
        UObject* WorldContextObject,
        FGameplayTag Tag,
        FName Data
    );

    UFUNCTION(BlueprintCallable, Category="Dynamic Storage|Internal", meta=(WorldContext="WorldContextObject", BlueprintInternalUseOnly="true"))
    static void RegisterTextData(
        UObject* WorldContextObject,
        FGameplayTag Tag,
        FText Data
    );

    UFUNCTION(BlueprintCallable, Category="Dynamic Storage|Internal", meta=(WorldContext="WorldContextObject", BlueprintInternalUseOnly="true"))
    static void RegisterByteData(
        UObject* WorldContextObject,
        FGameplayTag Tag,
        uint8 Data
    );

    UFUNCTION(BlueprintCallable, Category="Dynamic Storage|Internal", meta=(WorldContext="WorldContextObject", BlueprintInternalUseOnly="true"))
    static void RegisterObjectData(
        UObject* WorldContextObject,
        FGameplayTag Tag,
        UObject* Data
    );

    UFUNCTION(BlueprintCallable, Category="Dynamic Storage|Internal", meta=(WorldContext="WorldContextObject", BlueprintInternalUseOnly="true"))
    static void RegisterClassData(
        UObject* WorldContextObject,
        FGameplayTag Tag,
        UClass* Data
    );
    
    UFUNCTION(
        BlueprintCallable,
        CustomThunk,
        Category="Dynamic Storage|Internal",
        meta=(WorldContext="WorldContextObject", CustomStructureParam="Data", BlueprintInternalUseOnly="true")
    )
    static void RegisterStructData(
        UObject* WorldContextObject,
        FGameplayTag Tag,
        const int32& Data
    );
    
    UFUNCTION(
        BlueprintCallable,
        CustomThunk,
        Category="Dynamic Storage|Internal",
        meta=(WorldContext="WorldContextObject", CustomStructureParam="Data", BlueprintInternalUseOnly="true")
    )
    static void RegisterMapData(
        UObject* WorldContextObject,
        FGameplayTag Tag,
        const int32& Data
    );
    
    UFUNCTION(
        BlueprintCallable,
        CustomThunk,
        Category="Dynamic Storage|Internal",
        meta=(WorldContext="WorldContextObject", CustomStructureParam="Data", BlueprintInternalUseOnly="true")
    )
    static void RegisterArrayData(
        UObject* WorldContextObject,
        FGameplayTag Tag,
        const int32& Data
    );
    
    
    UFUNCTION(
        BlueprintCallable,
        CustomThunk,
        Category="Dynamic Storage|Internal",
        meta=(WorldContext="WorldContextObject", CustomStructureParam="Data", BlueprintInternalUseOnly="true")
    )
    static void RegisterSetData(
        UObject* WorldContextObject,
        FGameplayTag Tag,
        const int32& Data
    );
    
    UFUNCTION(BlueprintCallable, Category="Dynamic Storage|Internal", meta=(WorldContext="WorldContextObject", BlueprintInternalUseOnly="true"))
    static bool RetrieveBoolData(
        UObject* WorldContextObject,
        FGameplayTag Tag
    );
    
    UFUNCTION(BlueprintCallable, Category="Dynamic Storage|Internal", meta=(WorldContext="WorldContextObject", BlueprintInternalUseOnly="true"))
    static int32 RetrieveIntData(
        UObject* WorldContextObject,
        FGameplayTag Tag
    );
    
    UFUNCTION(BlueprintCallable, Category="Dynamic Storage|Internal", meta=(WorldContext="WorldContextObject", BlueprintInternalUseOnly="true"))
    static int64 RetrieveInt64Data(
        UObject* WorldContextObject,
        FGameplayTag Tag
    );
    
    UFUNCTION(BlueprintCallable, Category="Dynamic Storage|Internal", meta=(WorldContext="WorldContextObject", BlueprintInternalUseOnly="true"))
    static float RetrieveFloatData(
        UObject* WorldContextObject,
        FGameplayTag Tag
    );
    
    UFUNCTION(BlueprintCallable, Category="Dynamic Storage|Internal", meta=(WorldContext="WorldContextObject", BlueprintInternalUseOnly="true"))
    static double RetrieveDoubleData(
        UObject* WorldContextObject,
        FGameplayTag Tag
    );
    
    UFUNCTION(BlueprintCallable, Category="Dynamic Storage|Internal", meta=(WorldContext="WorldContextObject", BlueprintInternalUseOnly="true"))
    static FString RetrieveStringData(
        UObject* WorldContextObject,
        FGameplayTag Tag
    );
    
    UFUNCTION(BlueprintCallable, Category="Dynamic Storage|Internal", meta=(WorldContext="WorldContextObject", BlueprintInternalUseOnly="true"))
    static FName RetrieveNameData(
        UObject* WorldContextObject,
        FGameplayTag Tag
    );
    
    UFUNCTION(BlueprintCallable, Category="Dynamic Storage|Internal", meta=(WorldContext="WorldContextObject", BlueprintInternalUseOnly="true"))
    static FText RetrieveTextData(
        UObject* WorldContextObject,
        FGameplayTag Tag
    );
    
    UFUNCTION(BlueprintCallable, Category="Dynamic Storage|Internal", meta=(WorldContext="WorldContextObject", BlueprintInternalUseOnly="true"))
    static uint8 RetrieveByteData(
        UObject* WorldContextObject,
        FGameplayTag Tag
    );
    
    UFUNCTION(BlueprintCallable, Category="Dynamic Storage|Internal", meta=(WorldContext="WorldContextObject", BlueprintInternalUseOnly="true"))
    static UObject* RetrieveObjectData(
        UObject* WorldContextObject,
        FGameplayTag Tag
    );
    
    UFUNCTION(BlueprintCallable, Category="Dynamic Storage|Internal", meta=(WorldContext="WorldContextObject", BlueprintInternalUseOnly="true"))
    static UClass* RetrieveClassData(
        UObject* WorldContextObject,
        FGameplayTag Tag
    );

    UFUNCTION(
        BlueprintCallable,
        CustomThunk,
        Category="Dynamic Storage|Internal",
        meta=(WorldContext="WorldContextObject", CustomStructureParam="Data", BlueprintInternalUseOnly="true")
    )
    static void RetrievePropertyData(
        UObject* WorldContextObject,
        FGameplayTag Tag,
        int32& Data
    );

    DECLARE_FUNCTION(execRetrievePropertyData);
    
    DECLARE_FUNCTION(execRegisterSetData);

    DECLARE_FUNCTION(execRegisterMapData);
    
    DECLARE_FUNCTION(execRegisterStructData);
    
    DECLARE_FUNCTION(execRegisterArrayData);
};