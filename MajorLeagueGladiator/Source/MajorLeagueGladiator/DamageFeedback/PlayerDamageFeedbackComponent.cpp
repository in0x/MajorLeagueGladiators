// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "PlayerDamageFeedbackComponent.h"


UPlayerDamageFeedbackComponent::UPlayerDamageFeedbackComponent()
{}

void UPlayerDamageFeedbackComponent::BeginPlay()
{
	UPostProcessComponent* postProcessComp = GetOwner()->FindComponentByClass<UPostProcessComponent>();
	if (postProcessComp == nullptr)
	{
		UE_LOG(DebugLog, Warning, TEXT("MlgPlayerCharacter: UPostProcessComponent could not be found!"));
	}
}