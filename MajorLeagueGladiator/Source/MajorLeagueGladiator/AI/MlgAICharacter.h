// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "MlgAICharacter.generated.h"

class UHealthComponent;
class UWeakpointComponent;
class UTriggerZoneComponent;
class UDamageCauserComponent;
class UDamageReceiverComponent;
class UAIDamageFeedbackComponent;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AMlgAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMlgAICharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void ApplyStagger(float DurationSeconds);
	
protected:
	virtual float InternalTakePointDamage(float Damage, const FPointDamageEvent& PointDamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
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

	UPROPERTY()
	UAIDamageFeedbackComponent* damageFeedback;
};
	