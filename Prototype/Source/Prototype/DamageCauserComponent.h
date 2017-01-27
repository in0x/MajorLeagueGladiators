// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "DamageCauserComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROTOTYPE_API UDamageCauserComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDamageCauserComponent();
	virtual void BeginPlay() override;	

private:

	UFUNCTION()
	void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	void onCooldownReset();

	UPROPERTY(EditAnywhere, Category="Damage")
	float damageAppliedOnHit;

	UPROPERTY(EditAnywhere, Category = "Damage")
	TSubclassOf<UDamageType> damageType;

	UPROPERTY(EditAnywhere, Category = "Damage")
	float timeBeforeCanDamageAgainSeconds = 0.5;

	FTimerHandle cooldownTimerHandle;

	bool bCanCauseDamage;
};
