// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "DamageHandlerComponent.h"
#include "HealthComponent.h"

UDamageHandlerComponent::UDamageHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDamageHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UDamageHandlerComponent::HandleDamage);
	GetOwner()->OnTakePointDamage.AddDynamic(this, &UDamageHandlerComponent::HandlePointDamage);

	for (auto& healthComp : GetOwner()->GetComponentsByClass(UHealthComponent::StaticClass()))
	{
		healthComponents.Add(CastChecked<UHealthComponent>(healthComp));
	}
}

void UDamageHandlerComponent::HandleDamage(AActor* damagedActor, float damage, const UDamageType* damageType, AController* instigatedBy, AActor* damageCauser)
{
	UHealthComponent* healthComp = *healthComponents.FindByPredicate([](auto hc)
	{
		return hc->IsMainHealth;
	});
	
	if (healthComp)
	{
		healthComp->CurrentHealth -= damage;
		UE_LOG(LogTemp, Warning, TEXT("Apply damage | Health left: %f"), healthComp->CurrentHealth);

		if (healthComp->CurrentHealth <= 0.f)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("The actor '%s' has no health left!"), *damagedActor->GetName()));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldnt find MainHealth, cant apply damage"));
	}
}


void UDamageHandlerComponent::HandlePointDamage(AActor* damagedActor, float damage, class AController* instigatedBy, FVector hitLocation, class UPrimitiveComponent* hitComponent, FName boneName, FVector shotFromDirection, const UDamageType* damageType, AActor* damageCauser)
{
	float minDistance = std::numeric_limits<float>::max();
	UHealthComponent* closest = nullptr;

	for (auto healthComp : healthComponents)
	{
		if (healthComp->IsMainHealth)
			continue;

		float distance = (healthComp->GetComponentLocation() - hitLocation).Size();

		if (distance < minDistance)
		{
			minDistance = distance;
			closest = healthComp;
		}
	}

	if (closest)
	{
		closest->CurrentHealth -= damage;
		UE_LOG(LogTemp, Warning, TEXT("Apply damage | Health left: %f"), closest->CurrentHealth);

		if (closest->CurrentHealth <= 0.f)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("The actor '%s' has no health left!"), *damagedActor->GetName()));
		}
	}
}