// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgRangedWeapon.h"

namespace 
{
	const char* RANGED_WEAPON_COLLISION_PROFILE_NAME = "RangedWeapon";
}

AMlgRangedWeapon::AMlgRangedWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UPrimitiveComponent* rootPrimitve = CastChecked<UPrimitiveComponent>(RootComponent);
	rootPrimitve->SetSimulatePhysics(false);
	rootPrimitve->SetCollisionProfileName(RANGED_WEAPON_COLLISION_PROFILE_NAME);
}


