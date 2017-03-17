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
	splineMesh->SetupAttachment(GetRootComponent());

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

	PickTarget();

	auto path = predictResult.PathData;	
	check(path.GetData());
	auto highest = std::max_element(path.GetData(), path.GetData() + path.Num(), [](auto a, auto b) {return a.Location.Z < b.Location.Z; });

	auto dot = FVector::DotProduct(FVector(path[0].Velocity.X, path[0].Velocity.Y, 0).GetSafeNormal(), path[0].Velocity.GetSafeNormal());
	auto angle = FMath::Acos(dot);
	
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("%f"), FMath::RadiansToDegrees(angle)));

	FInterpCurve<FVector> curve;
	auto first = curve.AddPoint(path[0].Time, path[0].Location);
	curve.Points[first].InterpMode = CIM_CurveAuto;
	
	auto second = curve.AddPoint(highest->Time, highest->Location);
	curve.Points[second].InterpMode = CIM_CurveAuto;

	auto third = curve.AddPoint(path.Last().Time, path.Last().Location);
	curve.Points[third].InterpMode = CIM_CurveAuto;

	curve.AutoSetTangents(0.0f, false);
	
	auto begin = curve.Points[0].LeaveTangent;
	auto end = curve.Points[2].ArriveTangent;
	
	begin.Z *= 4.f * FMath::Sin(angle);
	end.Z *= 4.f * FMath::Sin(angle);
	
	splineMesh->SetStartAndEnd(GetTransform().InverseTransformPosition(curve.Points[0].OutVal),
		GetTransform().InverseTransformVector(begin),
		GetTransform().InverseTransformPosition(curve.Points[2].OutVal),
		GetTransform().InverseTransformVector(end));

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
