// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "DamageReceiverComponent.h"
#include "HealthComponent.h"

void UDamageReceiverComponent::BeginPlay()
{
	Super::BeginPlay();

	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UDamageReceiverComponent::HandleDamage);
	GetOwner()->OnTakePointDamage.AddDynamic(this, &UDamageReceiverComponent::HandlePointDamage);

	for (auto& healthComp : GetOwner()->GetComponentsByClass(UHealthComponent::StaticClass()))
	{
		healthComponents.Add(CastChecked<UHealthComponent>(healthComp));
	}
}

void UDamageReceiverComponent::HandleDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	UHealthComponent* healthComp = *healthComponents.FindByPredicate([](auto hc)
	{
		return hc->IsMainHealth;
	});
	
	if (healthComp)
	{
		healthComp->CurrentHealth -= Damage;
		UE_LOG(LogTemp, Warning, TEXT("Apply damage | Health left: %f"), healthComp->CurrentHealth);

		if (healthComp->CurrentHealth <= 0.f)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("The actor '%s' has no health left!"), *DamagedActor->GetName()));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldnt find MainHealth, cant apply damage"));
	}
}

void UDamageReceiverComponent::HandlePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, 
												UPrimitiveComponent* HitComponent, FName BoneName, FVector ShotFromDirection, 
												const UDamageType* DamageType, AActor* DamageCauser)
{
	float minDistance = std::numeric_limits<float>::max();
	UHealthComponent* closest = nullptr;

	for (auto healthComp : healthComponents)
	{
		if (healthComp->IsMainHealth)
			continue;

		float distance = (healthComp->GetComponentLocation() - HitLocation).Size();

		if (distance < minDistance)
		{
			minDistance = distance;
			closest = healthComp;
		}
	}

	if (closest)
	{
		closest->CurrentHealth -= Damage;
		UE_LOG(LogTemp, Warning, TEXT("Apply damage | Health left: %f"), closest->CurrentHealth);

		if (closest->CurrentHealth <= 0.f)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("The actor '%s' has no health left!"), *DamagedActor->GetName()));
		}
	}
}