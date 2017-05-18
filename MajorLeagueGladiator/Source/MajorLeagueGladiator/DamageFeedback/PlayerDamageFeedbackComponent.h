// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DamageFeedbackComponent.h"
#include "PlayerDamageFeedbackComponent.generated.h"

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MAJORLEAGUEGLADIATOR_API UPlayerDamageFeedbackComponent : public UDamageFeedbackComponent
{
	GENERATED_BODY()

public:
	UPlayerDamageFeedbackComponent();
	
	virtual void BeginPlay() override;
	
};
