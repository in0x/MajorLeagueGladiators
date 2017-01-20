// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRExpansion/VRPlayerController.h"
#include "PrototypePlayerController.generated.h"

class UPlayerHudWidget;
/**
 * 
 */
UCLASS()
class PROTOTYPE_API APrototypePlayerController : public AVRPlayerController
{
	GENERATED_BODY()
public:
	APrototypePlayerController();
	UPlayerHudWidget* GetHudWidget()
	{
		return hudWidget;
	};
	void InitHudWidget();

private:
	class UClass* hudWidgetClass;
	class UPlayerHudWidget* hudWidget;
};
