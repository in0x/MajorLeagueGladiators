// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "TutorialWallDisappearHealth.h"
#include "HealthComponent.h"

void ATutorialWallDisappearHealth::BeginPlay()
{
	Super::BeginPlay();

	ACharacter* character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	
	cachedPlayerHealthComponent = character->FindComponentByClass<UHealthComponent>();
	cachedPlayerHealthComponent->HealthChangedDelegate.AddDynamic(this, &ATutorialWallDisappearHealth::OnPlayerHealthChanged);
}

void ATutorialWallDisappearHealth::OnPlayerHealthChanged(float NewHealthPercentage, float OldHealthPercentage)
{
	if (NewHealthPercentage > OldHealthPercentage)
	{
		cachedPlayerHealthComponent->HealthChangedDelegate.RemoveDynamic(this, &ATutorialWallDisappearHealth::OnPlayerHealthChanged);
		Destroy();
	}
}
