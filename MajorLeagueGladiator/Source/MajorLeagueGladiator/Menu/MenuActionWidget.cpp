// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MenuActionWidget.h"
#include "TextWidget.h"

bool UMenuActionWidget::Initialize() 
{
	UE_LOG(DebugLog, Log, TEXT("UMenuActionWidget::Initialize()"));

	//UWidget* widget = GetWidgetFromName(FName("Text"));
	//UTextWidget* textWidget = CastChecked<UTextWidget>(widget);
	//textWidget->SetText(centerText);

	return Super::Initialize();
}

void UMenuActionWidget::SetMenuAction(EMenuAction::Type actionType)
{
	menuAction = actionType;
}

void UMenuActionWidget::TriggerMenuAction()
{
	OnMenuActionTriggered.Broadcast(menuAction.GetValue());
}

