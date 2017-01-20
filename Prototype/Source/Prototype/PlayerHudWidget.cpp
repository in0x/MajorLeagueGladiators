// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "PlayerHudWidget.h"
#include "PlayerCharacter.h"
#include "HealthComponent.h"


void UPlayerHudWidget::OnAttachPlayer(APlayerCharacter* currentPlayerCharacter) {
	//setup delegates
	UHealthComponent* curHealthComp = Cast<UHealthComponent>(currentPlayerCharacter->GetComponentByClass(UHealthComponent::StaticClass()));
	if (curHealthComp != nullptr) 
	{
		curHealthComp->HealthChangedDelegate.AddDynamic(this, &UPlayerHudWidget::setCurrentHealthPercentage);
	}
}

void UPlayerHudWidget::setCurrentHealthPercentage(float percentage) 
{
	this->healthPercentage = percentage;
}