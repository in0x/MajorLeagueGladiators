// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgWeapon.h"

namespace 
{
	const char* RANGED_WEAPON_COLLISION_PROFILE_NAME = "RangedWeapon";
}

AMlgWeapon::AMlgWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UPrimitiveComponent* rootPrimitve = CastChecked<UPrimitiveComponent>(RootComponent);
	rootPrimitve->SetSimulatePhysics(false);
	rootPrimitve->SetCollisionProfileName(RANGED_WEAPON_COLLISION_PROFILE_NAME);
}


