// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "MessageEndpoint.h"
#include "DamageReceiverComponent.generated.h"

class UHealthComponent;

//DECLARE_MULTICAST_DELEGATE_OneParam(DamagedReceivedDelegate, int32);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAJORLEAGUEGLADIATOR_API UDamageReceiverComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDamageReceiverComponent();
	virtual void BeginPlay() override;

	bool CanBeDamagedBy(const UDamageType* DamageType) const;

private:
	UFUNCTION()
	void handleDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
	
	UFUNCTION() 
	void handlePointDamage(AActor* DamagedActor, float Damage, AController* instigatedBy, FVector HitLocation, UPrimitiveComponent* HitComponent, 
									   FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser);
	
	TArray<UHealthComponent*> healthComponents;

	UPROPERTY(EditAnywhere, Category = "Damage")
	TArray<TSubclassOf<UDamageType>> damageableBy;

	FMessageEndpointPtr msgEndpoint;
};
