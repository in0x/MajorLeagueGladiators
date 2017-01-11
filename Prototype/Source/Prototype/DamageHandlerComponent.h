// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "HealthComponent.h"
#include "DamageHandlerComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOTYPE_API UDamageHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDamageHandlerComponent();
	virtual void BeginPlay() override;
	
protected:
	UFUNCTION() void HandleDamage(AActor* damagedActor, float damage, const UDamageType* damageType, AController* instigatedBy, AActor* damageCauser);

	UFUNCTION() void HandlePointDamage( AActor* damagedActor, float damage, AController* instigatedBy, FVector hitLocation, UPrimitiveComponent* hitComponent, 
										FName boneName, FVector shotFromDirection, const UDamageType* damageType, AActor* damageCauser);
	
	TArray<UHealthComponent*> healthComponents;
};
