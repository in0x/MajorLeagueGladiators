// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "DamageCauserComponent.h"

UDamageCauserComponent::UDamageCauserComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDamageCauserComponent::BeginPlay()
{
	Super::BeginPlay();
	GetOwner()->OnActorHit.AddDynamic(this, &UDamageCauserComponent::OnHit);
}

void UDamageCauserComponent::OnHit(AActor* selfActor, AActor* otherActor, FVector normalImpulse, const FHitResult& hit)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, TEXT("HIT"));

	auto gunDamageType = TSubclassOf<UDamageType>(UDamageType::StaticClass());
	
	UGameplayStatics::ApplyPointDamage(otherActor, 10.f, normalImpulse, hit, nullptr, selfActor, gunDamageType);
}