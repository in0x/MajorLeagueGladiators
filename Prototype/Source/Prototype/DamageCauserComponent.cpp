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

void UDamageCauserComponent::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	auto gunDamageType = TSubclassOf<UDamageType>(UDamageType::StaticClass());	
	UGameplayStatics::ApplyPointDamage(OtherActor, damageAppliedOnHit, NormalImpulse, Hit, nullptr, SelfActor, gunDamageType);
}