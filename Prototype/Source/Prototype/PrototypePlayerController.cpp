// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "PrototypePlayerController.h"
#include "PlayerHudWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

APrototypePlayerController::APrototypePlayerController()
{
	static ConstructorHelpers::FClassFinder<UPlayerHudWidget> hudWidgetObj(TEXT("/Game/BluePrints/PlayerHudBP"));	//get the BP class for the HUD BP
	checkf(hudWidgetObj.Succeeded(), TEXT("PlayerHUDBP class not found!"));
	hudWidgetClass = hudWidgetObj.Class;
}

void APrototypePlayerController::InitHudWidget(APlayerCharacter* CurrentPlayerCharacter)
{
	if (hudWidget == nullptr)
	{
		hudWidget = CreateWidget<UPlayerHudWidget>(this, hudWidgetClass);
		hudWidget->AddToViewport();
		hudWidget->OnAttachPlayer(CurrentPlayerCharacter);
	}
}




