// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "DamageCauserComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MAJORLEAGUEGLADIATOR_API UDamageCauserComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDamageCauserComponent();
	virtual void BeginPlay() override;	

	void SetDamageType(TSubclassOf<UDamageType> Type)
	{
		damageType = Type;
	}

protected:
	UFUNCTION()
	virtual void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UPROPERTY(EditAnywhere, Category = "Damage")
	float damageAppliedOnHit;

	UPROPERTY(EditAnywhere, Category = "Damage")
	TSubclassOf<UDamageType> damageType;
};
