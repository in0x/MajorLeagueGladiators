// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
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
	
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UDamageReceiverComponent::handleDamage);
	GetOwner()->OnTakePointDamage.AddDynamic(this, &UDamageReceiverComponent::handlePointDamage);

	for (auto& healthComp : GetOwner()->GetComponentsByClass(UHealthComponent::StaticClass()))
	{
		healthComponents.Add(CastChecked<UHealthComponent>(healthComp));
	}
}

void UDamageReceiverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Role %d, Health %f"), static_cast<int>(GetOwner()->Role.GetValue()), healthComponents[0]->GetCurrentHealth()));
}

void UDamageReceiverComponent::handleDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
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