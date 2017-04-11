// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "DamageCauserComponent.h"

#include "MlgGameplayStatics.h"


UDamageCauserComponent::UDamageCauserComponent()
	: damageAppliedOnHit(0.0f)
	, cooldownBetweenDamage(1.0f)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDamageCauserComponent::BeginPlay()
{
	Super::BeginPlay();
	GetOwner()->OnActorBeginOverlap.AddDynamic(this, &UDamageCauserComponent::OnBeginOverlap);
}

void UDamageCauserComponent::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	FTimerManager& timerManager = GetWorld()->GetTimerManager();

	if (timerManager.GetTimerRemaining(cooldownTimer) <= 0.f &&
		GetOwnerRole() >= ROLE_Authority && 
		UMlgGameplayStatics::CanDealDamageTo(OverlappedActor, OtherActor))
	{
		APawn* damageInstigatorPawn = Cast<APawn>(OverlappedActor);
		if (damageInstigatorPawn == nullptr)
		{
			damageInstigatorPawn = OverlappedActor->GetInstigator();
		}
		check(damageInstigatorPawn);

		UGameplayStatics::ApplyDamage(OtherActor, damageAppliedOnHit, damageInstigatorPawn->GetController(), OverlappedActor, DamageType);
		timerManager.SetTimer(cooldownTimer, cooldownBetweenDamage, false);
	}
}

