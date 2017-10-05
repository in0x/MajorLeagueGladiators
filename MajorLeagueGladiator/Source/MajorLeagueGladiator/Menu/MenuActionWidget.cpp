// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MenuActionWidget.h"

void UMenuActionWidget::SetMenuAction(EMenuAction::Type actionType)
{
	menuAction = actionType;
}

void UMenuActionWidget::TriggerMenuAction()
{
	OnMenuActionTriggered.Broadcast(menuAction.GetValue());
}

