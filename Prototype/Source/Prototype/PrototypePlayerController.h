// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRExpansion/VRPlayerController.h"
#include "PrototypePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE_API APrototypePlayerController : public AVRPlayerController
{
	GENERATED_BODY()
private:
	class UClass * m_hudWidgetClass;
	class UPlayerHudWidget * m_hudWidget;

public:
	APrototypePlayerController();
	UPlayerHudWidget* getHudWidget()
	{
		return m_hudWidget;
	};
	void initHudWidget();
};
