// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "PlayerHudWidget.h"

#include "HealthComponent.h"

void UPlayerHudWidget::SetCurrentPercentage(float newPercentage, float oldPercentage)
{
	CurrentPercentage = newPercentage;
}