// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "DamageHandlerComponent.h"
#include "HealthComponent.h"

// Sets default values for this component's properties
UDamageHandlerComponent::UDamageHandlerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	
	
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
	auto hComp = DamagedActor->GetComponentByClass(UHealthComponent::StaticClass());
	

	if (hComp)
	{
		UHealthComponent* healthComp = CastChecked<UHealthComponent>(hComp);

		healthComp->CurrentHealth -= Damage;
		UE_LOG(LogTemp, Warning, TEXT("Apply damage... | Health left: %f"), healthComp->CurrentHealth);
	}
}
