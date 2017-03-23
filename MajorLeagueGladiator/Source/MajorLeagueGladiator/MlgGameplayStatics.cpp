// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGameplayStatics.h"
#include "MlgPlayerState.h"
#include "Classes/Components/SplineMeshComponent.h"  

FSplineMeshComponentParams::FSplineMeshComponentParams()
	: Material(nullptr)
	, StaticMesh(nullptr)
	, StartScale(FVector2D(0, 0))
	, EndScale(FVector2D(0, 0))
	, SplineMeshAxis(ESplineMeshAxis::Type::X)
{}

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

	if (DamageInstigator->PlayerState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT(" \"%s\" has no PlayerState"), *DamageInstigator->GetName());
		return false;
	}

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

void UMlgGameplayStatics::ConfigureSplineMesh(USplineMeshComponent* splineMesh, const FSplineMeshComponentParams& params)
{
	check(splineMesh);

	splineMesh->SetMobility(EComponentMobility::Movable);

	if (params.Material)
	{
		splineMesh->SetMaterial(0, UMaterialInstanceDynamic::Create(params.Material, nullptr));
	}
	else
	{
		UE_LOG(DebugLog, Warning, TEXT("Material passed was null, spline will not have a material"));
	}

	if (params.StaticMesh)
	{
		splineMesh->SetStaticMesh(params.StaticMesh);
	}
	else
	{
		UE_LOG(DebugLog, Warning, TEXT("StaticMesh passed was null, spline will not have a static mesh"));
	}

	splineMesh->SetStartScale(params.StartScale);
	splineMesh->SetEndScale(params.EndScale);
	splineMesh->SetForwardAxis(params.SplineMeshAxis);
}

void UMlgGameplayStatics::SetSplineMeshFromPath(USplineMeshComponent* splineMesh, const FTransform& parentTransform, const TArray<FPredictProjectilePathPointData>& path)
{
	float timeInFlight = path.Last().Time; // Need to multiply tangents with this to properly take length into account.

	splineMesh->SetStartAndEnd(	parentTransform.InverseTransformPosition(path[0].Location),
								parentTransform.InverseTransformVector(path[0].Velocity * timeInFlight),
								parentTransform.InverseTransformPosition(path.Last().Location),
								parentTransform.InverseTransformVector(path.Last().Velocity * timeInFlight));

	splineMesh->UpdateMesh();
}