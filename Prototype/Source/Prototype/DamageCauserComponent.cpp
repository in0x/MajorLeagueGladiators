// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "DamageCauserComponent.h"


// Sets default values for this component's properties
UDamageCauserComponent::UDamageCauserComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UDamageCauserComponent::BeginPlay()
{
	Super::BeginPlay();
	GetOwner()->OnActorHit.AddDynamic(this, &UDamageCauserComponent::OnHit);
	// ...
	
}


// Called every frame
void UDamageCauserComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}


void UDamageCauserComponent::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, TEXT("HIT"));

	//Do some calc for damage...
	float damage = 10.f;
	TSubclassOf<UDamageType> const gunDamageType = TSubclassOf<UDamageType>(UDamageType::StaticClass()); //TODO GunDamageType, SwordDamageType, ...

	//UGameplayStatics::ApplyDamage(OtherActor, damage, nullptr, SelfActor, gunDamageType);
	UGameplayStatics::ApplyPointDamage(OtherActor, damage, NormalImpulse, Hit, nullptr, SelfActor, gunDamageType);
}