#include "MajorLeagueGladiator.h"
#include "GameplayAbilityTargetActor_PredictProjectile.h"
#include "Characters/MlgPlayerCharacter.h"
#include "TargetingSplineMeshComponent.h"
#include "Abilities/GameplayAbility.h"
#include "VRControllerComponent.h"
#include "PlayAreaMeshComponent.h"

AGameplayAbilityTargetActor_PredictProjectile::AGameplayAbilityTargetActor_PredictProjectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, TargetProjectileSpeed(1000.f)
	, TargetProjectileFlightTime(2.f)
	, bShowPlayArea(true)
	, OverrideGravityZ(0.f)
{
	PrimaryActorTick.bCanEverTick = true;

	TargetingSplineMesh = ObjectInitializer.CreateDefaultSubobject<UTargetingSplineMeshComponent>(this, TEXT("SplineMesh"));
	PlayAreaMesh = ObjectInitializer.CreateDefaultSubobject<UPlayAreaMeshComponent>(this, TEXT("PlayAreaMesh"));
}

void AGameplayAbilityTargetActor_PredictProjectile::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	AMlgPlayerCharacter* owner = CastChecked<AMlgPlayerCharacter>(Ability->GetOwningActorFromActorInfo());
	
	GetPlayerCapsuleFromAbility(owner);
	GetVrControllerFromAbility(owner);
}

void AGameplayAbilityTargetActor_PredictProjectile::SetTargetHitColor(FLinearColor Color)
{
	TargetingSplineMesh->TargetHitColor = Color;
}

void AGameplayAbilityTargetActor_PredictProjectile::SetTargetMissColor(FLinearColor Color)
{
	TargetingSplineMesh->TargetMissColor = Color;
}

void AGameplayAbilityTargetActor_PredictProjectile::GetPlayerCapsuleFromAbility(AMlgPlayerCharacter* owner)
{	
	playerCapsule = owner->GetCapsuleComponent();
	checkf(playerCapsule, TEXT("Could not get capsule from PlayerCharacter"));
}

void AGameplayAbilityTargetActor_PredictProjectile::GetVrControllerFromAbility(AMlgPlayerCharacter* owner)
{
	vrController = owner->GetMotionController(EControllerHand::Left);
	checkf(vrController, TEXT("Could not get controller mesh from PlayerCharacter"));
}

void AGameplayAbilityTargetActor_PredictProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	PickTarget();
	
	TargetingSplineMesh->SetFromProjectilePath(predictResult.PathData);
	TargetingSplineMesh->SetIsTargetValid(predictResult.HitResult.bBlockingHit);

	PlayAreaMesh->SetWorldLocation(TargetingSplineMesh->GetEndPositionWorld());
	PlayAreaMesh->SetIsTargetValid(bShowPlayArea && predictResult.HitResult.bBlockingHit);
}

bool AGameplayAbilityTargetActor_PredictProjectile::PickTarget()
{
	auto queryTypes = TArray<TEnumAsByte<EObjectTypeQuery>> {
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic)
	};

	FPredictProjectilePathParams params;

	if (targetingType == EPickMoveLocationTargeting::FromPlayerCapsule)
	{
		params = FPredictProjectilePathParams(0.f, playerCapsule->GetComponentLocation() - (playerCapsule->GetUpVector() * playerCapsule->GetScaledCapsuleHalfHeight()), 
			launchVelocity, TargetProjectileFlightTime);
	}
	else if (targetingType == EPickMoveLocationTargeting::FromController)
	{
		launchVelocity = vrController->GetForwardVector() * TargetProjectileSpeed;
		params = FPredictProjectilePathParams(0.f, vrController->GetComponentLocation(), launchVelocity, TargetProjectileFlightTime);
	}
	else // FromSourceComponent
	{
		FTransform targetingTransform = StartLocation.GetTargetingTransform();
		launchVelocity = targetingTransform.GetRotation().GetForwardVector() * TargetProjectileSpeed;
		params = FPredictProjectilePathParams(0.f, targetingTransform.GetLocation(), launchVelocity, TargetProjectileFlightTime);
	}

	params.ObjectTypes = queryTypes;
	params.ActorsToIgnore = IgnoredActors;
	params.bTraceComplex = false;
	params.bTraceWithCollision = true;

	if (OverrideGravityZ != 0.f)
	{
		params.OverrideGravityZ = OverrideGravityZ;
	}
	
	return UGameplayStatics::PredictProjectilePath(GetWorld(), params, predictResult);
}

bool AGameplayAbilityTargetActor_PredictProjectile::IsConfirmTargetingAllowed()
{
	return true;
}

FGameplayAbilityTargetDataHandle AGameplayAbilityTargetActor_PredictProjectile::makeDataHandle() 
{
	// Reuse TraceStart to transport launch velocity
	predictResult.HitResult.TraceStart = launchVelocity;

	return FGameplayAbilityTargetDataHandle(
		new FGameplayAbilityTargetData_SingleTargetHit(predictResult.HitResult)
	);
}

void AGameplayAbilityTargetActor_PredictProjectile::ConfirmTargetingAndContinue()
{
	// I'm explicetly not calling Super since it just broadcasts with an empty TargetDataHandle 

	if (IsConfirmTargetingAllowed())
	{
		if (predictResult.HitResult.bBlockingHit)
		{
			TargetDataReadyDelegate.Broadcast(makeDataHandle());
		}
		else
		{
			CancelTargeting();
		}
	}
}

FVector AGameplayAbilityTargetActor_PredictProjectile::GetVelocityFromTargetDataHandle(const FGameplayAbilityTargetDataHandle& Data)
{
	return Data.Data[0]->GetHitResult()->TraceStart;
}
