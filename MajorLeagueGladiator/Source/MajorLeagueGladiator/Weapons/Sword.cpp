// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "Sword.h"

namespace
{
	const char* MELEE_WEAPON_COLLISION_PROFILE_NAME = "MeleeWeapon";
}

ASword::ASword(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetStaticMeshComponent()->bGenerateOverlapEvents = true;
	GetStaticMeshComponent()->SetCollisionProfileName(MELEE_WEAPON_COLLISION_PROFILE_NAME);
}
