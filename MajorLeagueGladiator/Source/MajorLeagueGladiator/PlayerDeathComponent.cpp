// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "PlayerDeathComponent.h"

void UPlayerDeathComponent::OnPlayerDied_NetMulticast_Implementation()
{
	AActor* owner = GetOwner();
	owner->TeleportTo(AfterDeathTeleportPosition, owner->GetActorRotation());
}

