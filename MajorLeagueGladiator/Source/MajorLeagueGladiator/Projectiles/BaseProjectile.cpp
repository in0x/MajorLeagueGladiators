// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "BaseProjectile.h"

#include "MlgPlayerState.h"

// Sets default values
ABaseProjectile::ABaseProjectile()
	: damage(10.f)
{

}


bool ABaseProjectile::CanDealDamageTo(const APawn* otherPawn) const
{
	return otherPawn && CanDealDamageTo(otherPawn->GetController());
}

bool ABaseProjectile::CanDealDamageTo(const AController* otherController) const
{
	if (otherController == nullptr)
	{
		return false;
	}

	const AMlgPlayerState* otherPlayerState = CastChecked<AMlgPlayerState>(otherController->PlayerState);
	const AMlgPlayerState* myPlayerState = CastChecked<AMlgPlayerState>(GetInstigatorController()->PlayerState);

	return !myPlayerState->IsSameTeam(otherPlayerState);
}
