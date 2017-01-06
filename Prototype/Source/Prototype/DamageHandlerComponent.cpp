// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "DamageHandlerComponent.h"
#include "HealthComponent.h"

// Sets default values for this component's properties
UDamageHandlerComponent::UDamageHandlerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	
	
}


// Called when the game starts
void UDamageHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (GetOwner() != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Adding HandleDamage func..."));
		GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UDamageHandlerComponent::HandleDamage);
		GetOwner()->OnTakePointDamage.AddDynamic(this, &UDamageHandlerComponent::HandlePointDamage);
		health_components = TArray<UHealthComponent*>();
		TArray<UActorComponent*> healthcomps = GetOwner()->GetComponentsByClass(UHealthComponent::StaticClass());

		for (int i = 0; i < healthcomps.Num(); i++)
		{
			health_components.Add(CastChecked<UHealthComponent>(healthcomps[i]));
		}
	}
}


// Called every frame
void UDamageHandlerComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}


void UDamageHandlerComponent::HandleDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	//FPointDamageEvent pointDamageEvent = Cast<FPointDamageEvent>(DamageType);
	UHealthComponent* healthComp = *health_components.FindByPredicate([](auto hc)
	{
		return hc->IsMainHealth;
	});
	
	//auto hComp = DamagedActor->GetComponentByClass(UHealthComponent::StaticClass());
	

	if (healthComp)
	{
		//UHealthComponent* healthComp = CastChecked<UHealthComponent>(hComp);

		healthComp->CurrentHealth -= Damage;
		UE_LOG(LogTemp, Warning, TEXT("Apply damage... | Health left: %f"), healthComp->CurrentHealth);

		if (healthComp->CurrentHealth <= 0.f)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("The actor '%s' has no health left!"), *DamagedActor->GetName()));
		}
	}
}


void UDamageHandlerComponent::HandlePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser)
{

	//FPointDamageEvent pointDamageEvent = Cast<FPointDamageEvent>(DamageType);
	//auto hComp = DamagedActor->GetComponentByClass(UHealthComponent::StaticClass());
	float minDistance = FLT_MAX;
	UHealthComponent* closest = nullptr;

	for (int i = 0; i < health_components.Num();i++)
	{
		if (health_components[i]->IsMainHealth)
			continue;
		float distance = (health_components[i]->GetComponentLocation() - HitLocation).Size();
		if (distance < minDistance)
		{
			minDistance = distance;
			closest = health_components[i];
		}
	}



	if (closest)
	{
		//Uclosestonent* closest = CastChecked<Uclosestonent>(hComp);

		closest->CurrentHealth -= Damage;
		UE_LOG(LogTemp, Warning, TEXT("Apply damage... | Health left: %f"), closest->CurrentHealth);

		if (closest->CurrentHealth <= 0.f)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("The actor '%s' has no health left!"), *DamagedActor->GetName()));
		}
	}

}