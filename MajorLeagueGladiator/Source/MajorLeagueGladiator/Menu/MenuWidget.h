// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.generated.h"

/**
 *
 */
UCLASS()
class MAJORLEAGUEGLADIATOR_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY()
	UGridPanel* grid;

	UPROPERTY()
	TArray<UWidget*> widgets;
};
