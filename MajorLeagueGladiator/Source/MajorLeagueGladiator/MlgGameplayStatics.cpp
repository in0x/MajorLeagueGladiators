// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGameplayStatics.h"

#include "MlgPlayerState.h"


bool UMlgGameplayStatics::CanDealDamageTo(const AActor* DamageDealer, const AActor* DamageReceiver)
{
	check(DamageDealer);
	check(DamageReceiver);

	const APawn* DamageDealerPawn = Cast<APawn>(DamageDealer);
	if (DamageDealerPawn == nullptr)
	{
		DamageDealerPawn = DamageDealer->GetInstigator();
	}

	if (DamageDealerPawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Damage Dealer Pawn has no instigator"));
		return false;
	}

	return CanDealDamageTo(DamageDealerPawn, DamageReceiver);
}

bool UMlgGameplayStatics::CanDealDamageTo(const APawn* DamageInstigator, const AActor* DamageReceiver)
{
	check(DamageInstigator);
	check(DamageReceiver);

	const AMlgPlayerState* playerstate = CastChecked<AMlgPlayerState>(DamageInstigator->PlayerState);
	return playerstate && CanDealDamageTo(playerstate, DamageReceiver);
}

bool UMlgGameplayStatics::CanDealDamageTo(const AController* DamageInstigator, const AActor* DamageReceiver)
{	
	check(DamageInstigator);
	check(DamageReceiver);

	const AMlgPlayerState* DamageInstigatorState = CastChecked<AMlgPlayerState>(DamageInstigator->PlayerState);
	return CanDealDamageTo(DamageInstigatorState, DamageReceiver);
}

bool UMlgGameplayStatics::CanDealDamageTo(const AMlgPlayerState* DamageDealerState, const AActor* DamageReceiver)
{
	check(DamageDealerState);
	check(DamageReceiver);

	if (const APawn* DamageReceiverPawn = Cast<APawn>(DamageReceiver))
	{
		if (DamageReceiverPawn->PlayerState == nullptr)
		{
			// Can damage stuff without playerstates
			return false;
		}
		const AMlgPlayerState* DamageReceiverState = CastChecked<AMlgPlayerState>(DamageReceiverPawn->PlayerState);
		return CanDealDamageTo(DamageDealerState, DamageReceiverState);
	}

	// Forbid damaging non pawns,
	// we might want to change this in the future
	return false;
}

bool UMlgGameplayStatics::CanDealDamageTo(const AMlgPlayerState* DamageDealer, const AMlgPlayerState* DamageReceiver)
{
	check(DamageDealer);
	check(DamageReceiver);

	return !DamageDealer->IsSameTeam(DamageReceiver);
}
