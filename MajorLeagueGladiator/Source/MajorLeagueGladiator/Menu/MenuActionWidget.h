// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuAction.h"
#include "MenuActionWidget.generated.h"

UCLASS()
class MAJORLEAGUEGLADIATOR_API UMenuActionWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	FMenuActionDelegate OnMenuActionTriggered;

	void SetMenuAction(EMenuAction::Type actionType);

	UFUNCTION(BlueprintCallable)
	void TriggerMenuAction();

	/*UFunction(BluePrintCallable)
	void SetText(const FString& string);*/

	virtual bool Initialize() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextWidget")
	FString centerText;

private:
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EMenuAction::Type> menuAction;
};
