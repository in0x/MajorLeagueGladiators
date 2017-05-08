// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIDamageFeedbackComponent.h"
#include "MeleeAIDamageFeedbackComponent.generated.h"

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class MAJORLEAGUEGLADIATOR_API UMeleeAIDamageFeedbackComponent : public UAIDamageFeedbackComponent
{
	GENERATED_BODY()
	
public:

	UMeleeAIDamageFeedbackComponent();
	
};
