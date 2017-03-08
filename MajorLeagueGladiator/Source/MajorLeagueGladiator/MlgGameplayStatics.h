// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MlgGameplayStatics.generated.h"

class AMlgPlayerState;

UCLASS()
class MAJORLEAGUEGLADIATOR_API UMlgGameplayStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	static bool CanDealDamageTo(const AActor* DamageDealer, const AActor* DamageReceiver);
	static bool CanDealDamageTo(const APawn* DamageInstigator, const AActor* DamageReceiver);
	static bool CanDealDamageTo(const AController* DamageInstigator, const AActor* DamageReceiver);
	static bool CanDealDamageTo(const AMlgPlayerState* DamageDealerState, const AActor* DamageReceiver);
	static bool CanDealDamageTo(const AMlgPlayerState* DamageDealer, const AMlgPlayerState* DamageReceiver);
};
