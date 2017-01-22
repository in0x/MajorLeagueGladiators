// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "PlayerHudWidget.h"
#include "PlayerCharacter.h"
#include "HealthComponent.h"


void UPlayerHudWidget::OnAttachPlayer(APlayerCharacter* currentPlayerCharacter) 
{
	UHealthComponent* curHealthComp = Cast<UHealthComponent>(currentPlayerCharacter->GetComponentByClass(UHealthComponent::StaticClass()));
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