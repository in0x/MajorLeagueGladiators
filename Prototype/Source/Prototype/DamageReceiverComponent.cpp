// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "DamageReceiverComponent.h"
#include "HealthComponent.h"

void UDamageReceiverComponent::BeginPlay()
{
	Super::BeginPlay();

	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UDamageReceiverComponent::handleDamage);
	GetOwner()->OnTakePointDamage.AddDynamic(this, &UDamageReceiverComponent::handlePointDamage);

	for (auto& healthComp : GetOwner()->GetComponentsByClass(UHealthComponent::StaticClass()))
	{
		healthComponents.Add(CastChecked<UHealthComponent>(healthComp));
	}
}

void UDamageReceiverComponent::handleDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	UHealthComponent* healthComp = healthComponents[0];

	if (healthComp)
	{
		healthComp->DecreaseHealth(Damage);

		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("Apply damage | Health left: %f"), healthComp->CurrentHealth()));

		if (healthComp->CurrentHealth() == 0.f)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("The actor '%s' has no health left!"), *DamagedActor->GetName()));
		}
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
		UE_LOG(LogTemp, Warning, TEXT("Apply damage | Health left: %f"), closest->CurrentHealth());

		if (closest->CurrentHealth() == 0.f)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("The actor '%s' has no health left!")
				, *DamagedActor->GetName()));
		}
	}
}