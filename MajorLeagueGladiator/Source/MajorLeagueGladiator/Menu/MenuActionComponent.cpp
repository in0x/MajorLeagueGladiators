// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MenuActionComponent.h"

UMenuActionComponent::UMenuActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMenuActionComponent::TriggerMenuAction() const
{
	OnMenuActionTriggered.Broadcast(menuAction);
}

