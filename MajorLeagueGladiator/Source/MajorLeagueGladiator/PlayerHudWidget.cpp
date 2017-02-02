// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "PlayerHudWidget.h"
#include "MlgPlayerCharacter.h"
#include "HealthComponent.h"


void UPlayerHudWidget::OnAttachPlayer(AMlgPlayerCharacter* PlayerCharacter)
{
	UHealthComponent* curHealthComp = Cast<UHealthComponent>(PlayerCharacter->GetComponentByClass(UHealthComponent::StaticClass()));
	if (curHealthComp != nullptr) 
	{
		curHealthComp->HealthChangedDelegate.AddDynamic(this, &UPlayerHudWidget::setCurrentHealthPercentage);
		healthPercentage = curHealthComp->GetCurrentHealthPercentage();
	}
}

void UPlayerHudWidget::setCurrentHealthPercentage(float Percentage) 
{
	healthPercentage = Percentage;
}