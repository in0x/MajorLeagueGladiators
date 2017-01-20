// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRExpansion/VRPlayerController.h"
#include "PrototypePlayerController.generated.h"

class UPlayerHudWidget;
class APlayerCharacter;
class UClass;
/**
 * 
 */
UCLASS()
class PROTOTYPE_API APrototypePlayerController : public AVRPlayerController
{
	GENERATED_BODY()
public:
	APrototypePlayerController();
	void InitHudWidget(APlayerCharacter* CurrentPlayerCharacter);

private:
	UClass* hudWidgetClass;
	UPlayerHudWidget* hudWidget;
};
