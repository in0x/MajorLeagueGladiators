#include "MajorLeagueGladiator.h"
#include "GameplayAbilityTargetActor_PredictProjectile.h"
#include "Abilities/GameplayAbility.h"
#include "MlgPlayerCharacter.h"
#include "VRControllerComponent.h"

AGameplayAbilityTargetActor_PredictProjectile::AGameplayAbilityTargetActor_PredictProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGameplayAbilityTargetActor_PredictProjectile::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	GetPlayerCapsuleFromAbility(Ability);
	GetVrControllerFromAbility(Ability);
}

void AGameplayAbilityTargetActor_PredictProjectile::GetPlayerCapsuleFromAbility(UGameplayAbility* Ability)
{
	auto owner = Ability->GetOwningActorFromActorInfo();
	auto mlgPlayerCharacter = CastChecked<AMlgPlayerCharacter>(owner);
	
	playerCapsule = mlgPlayerCharacter->GetCapsuleComponent();
	checkf(playerCapsule, TEXT("Could not get capsule from PlayerCharacter"));
}

void AGameplayAbilityTargetActor_PredictProjectile::GetVrControllerFromAbility(UGameplayAbility* Ability)
{
	auto owner = Ability->GetOwningActorFromActorInfo();
	auto mlgPlayerCharacter = CastChecked<AMlgPlayerCharacter>(owner);

	vrController = mlgPlayerCharacter->GetMotionController(EControllerHand::Left);
	checkf(vrController, TEXT("Could not get controller mesh from PlayerCharacter"));
}

void AGameplayAbilityTargetActor_PredictProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FPredictProjectilePathResult result;
	FVector launchVel;

	PickTarget(result, launchVel);

	if (bShouldBroadcastResult)
	{
		TargetDataReadyDelegate.Broadcast(makeDataHandle(result, launchVel));
	}
}

bool AGameplayAbilityTargetActor_PredictProjectile::PickTarget(FPredictProjectilePathResult& OutResult, FVector& OutLaunchVelocity)
{
	auto queryTypes = TArray<TEnumAsByte<EObjectTypeQuery>> {
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic)
	};

	OutLaunchVelocity = vrController->GetForwardVector() * TargetProjectileSpeed;

	FPredictProjectilePathParams params;

	if (targetingType == EPickMoveLocationTargeting::FromPlayerCapsule)
	{
		params = FPredictProjectilePathParams(0.f, playerCapsule->GetComponentLocation() - (playerCapsule->GetUpVector() * playerCapsule->GetScaledCapsuleHalfHeight()), 
			OutLaunchVelocity, 2.f);
	}
	else // EPickMoveLocationTargeting == EPickMoveLocationTargeting::FromController
	{
		params = FPredictProjectilePathParams(0.f, vrController->GetComponentLocation(), OutLaunchVelocity, 2.f);
	}

	params.ObjectTypes = queryTypes;
	params.ActorsToIgnore = IgnoredActors;
	params.DrawDebugType = EDrawDebugTrace::ForOneFrame;
	params.DrawDebugTime = 1.f;
	params.bTraceComplex = false;
	params.bTraceWithCollision = true;

	return UGameplayStatics::PredictProjectilePath(GetWorld(), params, OutResult);
}

bool AGameplayAbilityTargetActor_PredictProjectile::IsConfirmTargetingAllowed()
{
	return false;
}

FGameplayAbilityTargetDataHandle AGameplayAbilityTargetActor_PredictProjectile::makeDataHandle(FPredictProjectilePathResult& PredictResult, const FVector& LaunchVelocity) const
{
	// Reuse TraceStart to transport launch velocity
	PredictResult.HitResult.TraceStart = LaunchVelocity;

	return FGameplayAbilityTargetDataHandle(
		new FGameplayAbilityTargetData_SingleTargetHit(PredictResult.HitResult)
	);
}