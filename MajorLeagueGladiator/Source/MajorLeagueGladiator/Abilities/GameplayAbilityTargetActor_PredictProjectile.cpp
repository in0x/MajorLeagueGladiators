#include "MajorLeagueGladiator.h"
#include "GameplayAbilityTargetActor_PredictProjectile.h"
#include "Abilities/GameplayAbility.h"
#include "Characters/MlgPlayerCharacter.h"
#include "VRControllerComponent.h"
#include "Classes/Components/SplineMeshComponent.h"  
#include "MlgGameplayStatics.h"

AGameplayAbilityTargetActor_PredictProjectile::AGameplayAbilityTargetActor_PredictProjectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, TargetProjectileSpeed(1000.f)
	, TargetProjectileFlightTime(2.f)
{
	PrimaryActorTick.bCanEverTick = true;

	splineMesh = ObjectInitializer.CreateDefaultSubobject<USplineMeshComponent>(this, TEXT("SplineMesh"));
	
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> splineMaterial(TEXT("Material'/Game/Materials/M_SplineArcMat.M_SplineArcMat'"));
	splineMeshMat = splineMaterial.Object;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> splineStaticMeshObject(TEXT("StaticMesh'/Game/MVRCFPS_Assets/BeamMesh.BeamMesh'"));
	splineStaticMesh = splineStaticMeshObject.Object;
}

void AGameplayAbilityTargetActor_PredictProjectile::BeginPlay()
{
	Super::BeginPlay();

	FSplineMeshComponentParams params;
	params.Material = splineMeshMat;
	params.StaticMesh = splineStaticMesh;
	params.StartScale = FVector2D(5, 5);
	params.EndScale = FVector2D(5, 5);
	
	UMlgGameplayStatics::ConfigureSplineMesh(splineMesh, params);
}

void AGameplayAbilityTargetActor_PredictProjectile::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	AMlgPlayerCharacter* owner = CastChecked<AMlgPlayerCharacter>(Ability->GetOwningActorFromActorInfo());
	
	GetPlayerCapsuleFromAbility(owner);
	GetVrControllerFromAbility(owner);

	FAttachmentTransformRules attachRules(EAttachmentRule::SnapToTarget, false);
	splineMesh->AttachToComponent(owner->GetRootComponent(), attachRules);
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
	UMlgGameplayStatics::SetSplineMeshFromPath(splineMesh, GetTransform(), predictResult.PathData);
}

bool AGameplayAbilityTargetActor_PredictProjectile::PickTarget()
{
	auto queryTypes = TArray<TEnumAsByte<EObjectTypeQuery>> {
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic)
	};

	launchVelocity = vrController->GetForwardVector() * TargetProjectileSpeed;

	FPredictProjectilePathParams params;

	if (targetingType == EPickMoveLocationTargeting::FromPlayerCapsule)
	{
		params = FPredictProjectilePathParams(0.f, playerCapsule->GetComponentLocation() - (playerCapsule->GetUpVector() * playerCapsule->GetScaledCapsuleHalfHeight()), 
			launchVelocity, TargetProjectileFlightTime);
	}
	else // EPickMoveLocationTargeting == EPickMoveLocationTargeting::FromController
	{
		params = FPredictProjectilePathParams(0.f, vrController->GetComponentLocation(), launchVelocity, 2.f);
	}

	params.ObjectTypes = queryTypes;
	params.ActorsToIgnore = IgnoredActors;
	params.bTraceComplex = false;
	params.bTraceWithCollision = true;

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
