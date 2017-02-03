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

	// between 0 and 1
	// defines how fast new sword speed influences overall speedvalue
	UPROPERTY(EditAnywhere)
	float slashVelocityLearnRate; 

	float calcBonusDamageFactor();
};
