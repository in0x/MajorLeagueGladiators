// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "DamageReceiverComponent.h"
#include "HealthComponent.h"

UDamageReceiverComponent::UDamageReceiverComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = true;
	SetIsReplicated(true);
}

void UDamageReceiverComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetOwner()->HasAuthority())
	{
		GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UDamageReceiverComponent::handleDamage);
		GetOwner()->OnTakePointDamage.AddDynamic(this, &UDamageReceiverComponent::handlePointDamage);

		for (auto& healthComp : GetOwner()->GetComponentsByClass(UHealthComponent::StaticClass()))
		{
			healthComponents.Add(CastChecked<UHealthComponent>(healthComp));
		}
	}
}

bool UDamageReceiverComponent::CanBeDamagedBy(const UDamageType* DamageType) const
{
	return damageableBy.ContainsByPredicate([&DamageType](TSubclassOf<UDamageType> currentDamageType)
	{
		return DamageType->GetClass() == *currentDamageType;
	});
}

void UDamageReceiverComponent::handleDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!CanBeDamagedBy(DamageType))
	{
		return;
	}

	OnDamageReceived.Broadcast(DamagedActor);

	UHealthComponent* healthComp = healthComponents[0];

	if (healthComp)
	{
		healthComp->DecreaseHealth(Damage);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldnt find HealthComponent, cant apply damage"));
	}


}

void UDamageReceiverComponent::handlePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, 
												UPrimitiveComponent* HitComponent, FName BoneName, FVector ShotFromDirection, 
												const UDamageType* DamageType, AActor* DamageCauser)
{
	if (!CanBeDamagedBy(DamageType))
	{
		return;
	}

	OnPointDamageReceived.Broadcast(DamagedActor, HitLocation);

	if (healthComponents.Num() > 1) {
		float minDistance = std::numeric_limits<float>::max();
		UHealthComponent* closest = nullptr;

		for (auto healthComp : healthComponents)
		{
			float distance = (healthComp->GetComponentLocation() - HitLocation).Size();

			if (distance < minDistance)
			{
				minDistance = distance;
				closest = healthComp;
			}
		}

		if (closest)
		{
			closest->DecreaseHealth(Damage);
		}
	}
}