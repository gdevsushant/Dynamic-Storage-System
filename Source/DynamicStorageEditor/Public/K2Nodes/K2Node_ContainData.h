// Copyright (c) Sushant 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_ContainData.generated.h"

UCLASS()
class DYNAMICSTORAGEEDITOR_API UK2Node_ContainData : public UK2Node
{
	GENERATED_BODY()

public:

	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;

	virtual FText GetMenuCategory() const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

private:

	static const FName ExecPinName;
	static const FName ThenPinName;
	static const FName WorldContextPinName;
	static const FName TagPinName;
	static const FName ReturnValuePinName;
};