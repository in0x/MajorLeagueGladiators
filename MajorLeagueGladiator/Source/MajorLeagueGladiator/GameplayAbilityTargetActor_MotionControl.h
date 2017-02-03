#pragma once

#include "Abilities/GameplayAbilityTargetActor.h"

#include "GameplayAbilityTargetActor_MotionControl.generated.h"

UCLASS()
class MAJORLEAGUEGLADIATOR_API AGameplayAbilityTargetActor_MotionControl : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
public:

	AGameplayAbilityTargetActor_MotionControl(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void ConfirmTargetingAndContinue() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	FVector getPointingDirection() const;
	FVector calculateTargetPosition() const;
};