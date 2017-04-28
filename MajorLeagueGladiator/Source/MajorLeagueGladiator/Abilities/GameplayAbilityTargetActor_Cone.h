#pragma once
// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/GameplayAbilityTargetActor.h"
#include "GameplayAbilityTargetActor_Cone.generated.h"

class USceneComponent;
class UTargetingSplineMeshComponent;

using FilterFunction = bool(AActor*);

UCLASS()
class MAJORLEAGUEGLADIATOR_API AGameplayAbilityTargetActor_Cone : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
public:
	AGameplayAbilityTargetActor_Cone(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	float HalfAngleDegrees;
	float Distance;
	bool IsVisualizingCone;
	bool AutoConfirm;

	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<AActor>> IgnoredActors;

	TArray<TEnumAsByte<EObjectTypeQuery>> QueryTypes;
	ECollisionChannel CollisionChannel;
	
	virtual void Tick(float DeltaSeconds) override;

	virtual void StartTargeting(UGameplayAbility* Ability) override;

	virtual void ConfirmTargetingAndContinue() override;

	FilterFunction* FilterFunctionPtr;

private:
	UTargetingSplineMeshComponent* coneVisualizer;

	TArray<TWeakObjectPtr<AActor>> Trace() const;
	FGameplayAbilityTargetDataHandle MakeTargetHandle(const TArray<TWeakObjectPtr<AActor>> Actors) const;

	float CalcEndConeScale() const;



};
