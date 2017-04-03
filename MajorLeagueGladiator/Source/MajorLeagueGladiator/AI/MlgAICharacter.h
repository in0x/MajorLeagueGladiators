// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "MlgAICharacter.generated.h"

class UHealthComponent;
class UWeakpointComponent;
class UTriggerZoneComponent;
class UDamageCauserComponent;
class UDamageReceiverComponent;
class UDamageVisualizerComponent;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AMlgAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMlgAICharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	virtual float InternalTakePointDamage(float Damage, const FPointDamageEvent& PointDamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual float InternalTakeRadialDamage(float Damage, const FRadialDamageEvent& RadialDamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
private:
	UPROPERTY(EditAnywhere)
	UHealthComponent* health;

	UPROPERTY(EditAnywhere)
	UWeakpointComponent* weakpoints;

	UPROPERTY(EditAnywhere)
	UTriggerZoneComponent* triggerZone;
	
	UPROPERTY(EditAnywhere)
	UDamageCauserComponent* damageCauser;
	
	UPROPERTY(EditAnywhere)
	UDamageReceiverComponent* damageReciever;

	UPROPERTY(EditAnywhere)
	UDamageVisualizerComponent* damageVisualizer;
};
	