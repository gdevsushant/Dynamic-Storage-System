// Copyright (c) Sushant 2026. All Rights Reserved.

#include "K2Nodes/K2Node_UnregisterData.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraphSchema_K2.h"
#include "GameplayTagContainer.h"
#include "KismetCompiler.h"
#include "Blueprint/DSBlueprintStorageBridge.h" 
#include "K2Node_CallFunction.h"

const FName UK2Node_UnregisterData::ExecPinName(TEXT("Execute"));
const FName UK2Node_UnregisterData::ThenPinName(TEXT("Then"));
const FName UK2Node_UnregisterData::WorldContextPinName(TEXT("WorldContextObject"));
const FName UK2Node_UnregisterData::TagPinName(TEXT("Tag"));
const FName UK2Node_UnregisterData::ReturnValuePinName(TEXT("ReturnValue"));

void UK2Node_UnregisterData::AllocateDefaultPins()
{
    CreatePin(
        EGPD_Input,
        UEdGraphSchema_K2::PC_Exec,
        ExecPinName
    );

    CreatePin(
        EGPD_Output,
        UEdGraphSchema_K2::PC_Exec,
        ThenPinName
    );

    UEdGraphPin* WorldContextPin = CreatePin(
        EGPD_Input,
        UEdGraphSchema_K2::PC_Object,
        UObject::StaticClass(),
        WorldContextPinName
    );

    if (WorldContextPin)
    {
        WorldContextPin->bHidden = false;
        WorldContextPin->PinFriendlyName = FText::FromString(TEXT("World Context Object"));
    }

    CreatePin(
        EGPD_Input,
        UEdGraphSchema_K2::PC_Struct,
        FGameplayTag::StaticStruct(),
        TagPinName
    );

    CreatePin(
        EGPD_Output,
        UEdGraphSchema_K2::PC_Boolean,
        ReturnValuePinName
    );
}

FText UK2Node_UnregisterData::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return FText::FromString(TEXT("Unregister Data"));
}

FText UK2Node_UnregisterData::GetTooltipText() const
{
    return FText::FromString(TEXT("Removes data from Dynamic Storage using a GameplayTag."));
}

FText UK2Node_UnregisterData::GetMenuCategory() const
{
    return FText::FromString(TEXT("Dynamic Storage"));
}

void UK2Node_UnregisterData::GetMenuActions(
    FBlueprintActionDatabaseRegistrar& ActionRegistrar
) const
{
    UClass* ActionKey = GetClass();

    if (ActionRegistrar.IsOpenForRegistration(ActionKey))
    {
        UBlueprintNodeSpawner* NodeSpawner =
            UBlueprintNodeSpawner::Create(GetClass());

        check(NodeSpawner);

        ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
    }
}

void UK2Node_UnregisterData::ExpandNode(
    FKismetCompilerContext& CompilerContext,
    UEdGraph* SourceGraph
)
{
    Super::ExpandNode(CompilerContext, SourceGraph);

    UK2Node_CallFunction* CallNode =
        CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(
            this,
            SourceGraph
        );

    CallNode->FunctionReference.SetExternalMember(
        GET_FUNCTION_NAME_CHECKED(UDSBlueprintStorageBridge, UnregisterData),
        UDSBlueprintStorageBridge::StaticClass()
    );

    CallNode->AllocateDefaultPins();

    CompilerContext.MovePinLinksToIntermediate(
        *FindPinChecked(ExecPinName),
        *CallNode->GetExecPin()
    );

    CompilerContext.MovePinLinksToIntermediate(
        *FindPinChecked(ThenPinName),
        *CallNode->GetThenPin()
    );

    CompilerContext.MovePinLinksToIntermediate(
        *FindPinChecked(WorldContextPinName),
        *CallNode->FindPinChecked(TEXT("WorldContextObject"))
    );

    CompilerContext.MovePinLinksToIntermediate(
        *FindPinChecked(TagPinName),
        *CallNode->FindPinChecked(TEXT("Tag"))
    );

    CompilerContext.MovePinLinksToIntermediate(
        *FindPinChecked(ReturnValuePinName),
        *CallNode->GetReturnValuePin()
    );

    BreakAllNodeLinks();
}