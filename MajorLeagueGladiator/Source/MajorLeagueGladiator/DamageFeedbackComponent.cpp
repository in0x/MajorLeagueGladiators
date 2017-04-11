// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "DamageFeedbackComponent.h"


UDamageFeedbackComponent::UDamageFeedbackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDamageFeedbackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (FDamageVisual& visual : visuals)
	{
		float scaledValue = (visual.MatVisDuration - visual.CurrentMatVisDuration) / visual.MatVisDuration;
		visual.MatInstance->SetScalarParameterValue(visual.DamageValParamName, FMath::Clamp(scaledValue, 0.f, 1.f));
		visual.CurrentMatVisDuration += DeltaTime;
	}

	visuals.RemoveAll([](FDamageVisual& visual)
	{
		if (visual.CurrentMatVisDuration >= visual.MatVisDuration)
		{
			visual.MatInstance->SetScalarParameterValue(visual.DamageValParamName, 0.0f);
			return true;
		}
		return false;
	});
}

