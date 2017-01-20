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
	static ConstructorHelpers::FClassFinder<UPlayerHudWidget> hudWidgetObj(TEXT("/Game/BluePrints/PlayerHUDBP"));	//get the BP class for the HUD BP
	if (hudWidgetObj.Succeeded()) {
		m_hudWidgetClass = hudWidgetObj.Class;
	}
	else {
		// hudWidgetObj not found
		m_hudWidgetClass = nullptr;
	}
}

void APrototypePlayerController::initHudWidget()
{
	if (m_hudWidget == nullptr && m_hudWidgetClass != nullptr)
	{
		m_hudWidget = CreateWidget<UPlayerHudWidget>(this, this->m_hudWidgetClass);
		m_hudWidget->AddToViewport();
	}
}




