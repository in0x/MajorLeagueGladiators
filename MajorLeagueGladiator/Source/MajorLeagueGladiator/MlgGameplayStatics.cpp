// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGameplayStatics.h"

#include "MlgPlayerState.h"


bool UMlgGameplayStatics::CanDealDamageTo(const AActor* DamageDealer, const AActor* DamageReceiver)
{
	const APawn* DamageDealerPawn = Cast<APawn>(DamageDealer);
	if (DamageDealerPawn == nullptr)
	{
		DamageDealerPawn = DamageDealer->GetInstigator();
	}
	check(DamageDealerPawn);

	return CanDealDamageTo(DamageDealerPawn, DamageReceiver);
}

bool UMlgGameplayStatics::CanDealDamageTo(const APawn* DamageInstigator, const AActor* DamageReceiver)
{
	check(DamageInstigator);
	const AController* controller = DamageInstigator->GetController();

	return controller && CanDealDamageTo(controller, DamageReceiver);
}

bool UMlgGameplayStatics::CanDealDamageTo(const AController* DamageInstigator, const AActor* DamageReceiver)
{
	if (const APawn* DamageReceiverPawn = Cast<APawn>(DamageReceiver))
	{
		const AMlgPlayerState* DamageInstigatorState = CastChecked<AMlgPlayerState>(DamageInstigator->PlayerState);
		const AMlgPlayerState* DamageReceiverState = CastChecked<AMlgPlayerState>(DamageReceiverPawn->GetController()->PlayerState);
		return CanDealDamageTo(DamageInstigatorState, DamageReceiverState);
	}

	// Forbid damaging non pawns,
	// we might want to change this in the future
	return false;
}

bool UMlgGameplayStatics::CanDealDamageTo(const AMlgPlayerState* DamageDealer, const AMlgPlayerState* DamageReceiver)
{
	return !DamageDealer->IsSameTeam(DamageReceiver);
}
