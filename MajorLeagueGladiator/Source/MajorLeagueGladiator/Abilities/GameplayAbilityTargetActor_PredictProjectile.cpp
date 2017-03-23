#include "MajorLeagueGladiator.h"
#include "GameplayAbilityTargetActor_PredictProjectile.h"
#include "Abilities/GameplayAbility.h"
#include "Characters/MlgPlayerCharacter.h"
#include "VRControllerComponent.h"
#include "Classes/Components/SplineMeshComponent.h"  
#include "Math/InterpCurve.h"
#include <numeric>

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

	auto instance = UMaterialInstanceDynamic::Create(splineMeshMat, this);
	splineMesh->SetMaterial(0, instance);
	splineMesh->SetMobility(EComponentMobility::Movable);

	splineMesh->SetStaticMesh(splineStaticMesh);

	splineMesh->SetStartScale(FVector2D(20, 5));
	splineMesh->SetEndScale(FVector2D(20, 5));

	splineMesh->SetForwardAxis(ESplineMeshAxis::Type::X);
}

void AGameplayAbilityTargetActor_PredictProjectile::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	AMlgPlayerCharacter* owner = CastChecked<AMlgPlayerCharacter>(Ability->GetOwningActorFromActorInfo());
	
	GetPlayerCapsuleFromAbility(owner);
	GetVrControllerFromAbility(owner);

	FAttachmentTransformRules attachRules(EAttachmentRule::SnapToTarget, false);
	USceneComponent* ownerRoot = owner->GetRootComponent();
		
	splineMesh->AttachToComponent(ownerRoot, attachRules);
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

	auto& path = predictResult.PathData;
	float timeInFlight = path.Last().Time; // Need to multiply tangents with this to properly take length into account.

	FTransform trafo = GetTransform();

	splineMesh->SetStartAndEnd(trafo.InverseTransformPosition(path[0].Location),
		trafo.InverseTransformVector(path[0].Velocity * timeInFlight),
		trafo.InverseTransformPosition(path.Last().Location),
		trafo.InverseTransformVector(path.Last().Velocity * timeInFlight));

	splineMesh->UpdateMesh();
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
	params.DrawDebugType = EDrawDebugTrace::ForOneFrame;
	params.DrawDebugTime = 1.f;
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
	// I'm explicetly not calling Super since it just broadcast with an empty TargetDataHandle 

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
