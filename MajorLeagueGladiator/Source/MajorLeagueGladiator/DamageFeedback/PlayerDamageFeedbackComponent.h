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

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void DoGlitchEffect();
	
	void DoRumble();

protected:

	UPROPERTY(EditAnywhere)
	UPostProcessComponent* postProcessComp;
	
	UPROPERTY(EditAnywhere)
	UMaterialInterface* postProcessGlitchMaterial;
	
	UPROPERTY(EditAnywhere)
	float hitDurationGlitch;
	
	UPROPERTY(EditAnywhere)
	float leftHitDurationGlitch;
	
	UPROPERTY(EditAnywhere)
	float transitionDurationGlitch;
};
