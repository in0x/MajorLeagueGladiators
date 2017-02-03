// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DamageCauserComponent.h"
#include "SwordDamageCauserComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MAJORLEAGUEGLADIATOR_API USwordDamageCauserComponent : public UDamageCauserComponent
{
	GENERATED_BODY()

public:
	USwordDamageCauserComponent();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
protected:
	virtual bool CanDoDamage() override;
	virtual float CalculateDamage() override;
private:
	FVector oldSwingSpeed;

	UPROPERTY(EditAnywhere)
	int threshholdDoDamageSquared;

	UPROPERTY(EditAnywhere)
	int threshholdBonusDamageSquared;

	UPROPERTY(EditAnywhere)
	float bonusDamageFactor;

	// 1 = only use current velocity*deltaTime, 0.1 = use 0.1 of current and 0.9 of previous velocity*deltaTime
	UPROPERTY(EditAnywhere)
	float slashDetectionAlpha; 

	float calcBonusDamageFactor();
};
