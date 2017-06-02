// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MenuActionComponent.h"

UMenuActionComponent::UMenuActionComponent()
{
}

void UMenuActionComponent::TriggerMenuAction() const
{
	OnMenuActionTriggered.Broadcast(menuAction);
}

void UMenuActionComponent::BeginPlay()
{
	UActorComponent::BeginPlay();
	AActor* owner = GetOwner();
	owner->OnActorHit.AddDynamic(this, &UMenuActionComponent::onActorHit);
	owner->OnActorBeginOverlap.AddDynamic(this, &UMenuActionComponent::onActorOverlap);
}

void UMenuActionComponent::onActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	TriggerMenuAction();
}

void UMenuActionComponent::onActorOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	TriggerMenuAction();
}

