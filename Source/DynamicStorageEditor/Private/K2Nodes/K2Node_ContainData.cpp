// Copyright (c) Sushant 2026. All Rights Reserved.

#include "K2Nodes/K2Node_ContainData.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraphSchema_K2.h"
#include "GameplayTagContainer.h"
#include "KismetCompiler.h"
#include "Blueprint/DSBlueprintStorageBridge.h"
#include "K2Node_CallFunction.h"

const FName UK2Node_ContainData::ExecPinName(TEXT("Execute"));
const FName UK2Node_ContainData::ThenPinName(TEXT("Then"));
const FName UK2Node_ContainData::WorldContextPinName(TEXT("WorldContextObject"));
const FName UK2Node_ContainData::TagPinName(TEXT("Tag"));
const FName UK2Node_ContainData::ReturnValuePinName(TEXT("ReturnValue"));

void UK2Node_ContainData::AllocateDefaultPins()
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

FText UK2Node_ContainData::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return FText::FromString(TEXT("Contain Data"));
}

FText UK2Node_ContainData::GetTooltipText() const
{
    return FText::FromString(TEXT("Checks whether Dynamic Storage contains data for the given GameplayTag."));
}

FText UK2Node_ContainData::GetMenuCategory() const
{
    return FText::FromString(TEXT("Dynamic Storage"));
}

void UK2Node_ContainData::GetMenuActions(
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

void UK2Node_ContainData::ExpandNode(
    FKismetCompilerContext& CompilerContext,
    UEdGraph* SourceGraph
)
{
    Super::ExpandNode(CompilerContext, SourceGraph);

    const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();

    UK2Node_CallFunction* CallNode =
        CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(
            this,
            SourceGraph
        );

    CallNode->FunctionReference.SetExternalMember(
        GET_FUNCTION_NAME_CHECKED(UDSBlueprintStorageBridge, ContainData),
        UDSBlueprintStorageBridge::StaticClass()
    );

    CallNode->AllocateDefaultPins();

    // Exec
    CompilerContext.MovePinLinksToIntermediate(
        *FindPinChecked(ExecPinName),
        *CallNode->GetExecPin()
    );

    CompilerContext.MovePinLinksToIntermediate(
        *FindPinChecked(ThenPinName),
        *CallNode->GetThenPin()
    );

    // WorldContext
    CompilerContext.MovePinLinksToIntermediate(
        *FindPinChecked(WorldContextPinName),
        *CallNode->FindPinChecked(TEXT("WorldContextObject"))
    );

    // Tag
    CompilerContext.MovePinLinksToIntermediate(
        *FindPinChecked(TagPinName),
        *CallNode->FindPinChecked(TEXT("Tag"))
    );

    // ReturnValue
    CompilerContext.MovePinLinksToIntermediate(
        *FindPinChecked(ReturnValuePinName),
        *CallNode->GetReturnValuePin()
    );

    BreakAllNodeLinks();
}