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
	static bool ComponentIsDamageableFrom(UPrimitiveComponent* VictimComp, FVector const& Origin, AActor const* IgnoredActor, const TArray<AActor*>& IgnoreActors, ECollisionChannel TraceChannel, FHitResult& OutHitResult);
	static bool ApplyRadialDamageWithFalloff(const UObject* WorldContextObject, float BaseDamage, float MinimumDamage, const FVector& Origin, float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff, TSubclassOf<class UDamageType> DamageTypeClass, AActor* DamageCauser, AController* InstigatedByController, bool bDrawDebug = false, const TArray<AActor*>& IgnoreActors = TArray<AActor*>(), ECollisionChannel DamagePreventionChannel = ECC_Visibility);
};
