// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "HitScanGunActor.h"
#include "DamageTypes/PlayerDamage.h"

namespace
{
	FBPActorGripInformation gripInfo;
}

AHitScanGunActor::AHitScanGunActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	shotAudioComponent = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("ShotAudioComponent"));
	laserMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("LaserMeshComponent"));
	laserMesh->SetupAttachment(GetStaticMeshComponent(), FName("ProjectileSpawn"));
}

void AHitScanGunActor::BeginPlay()
{
	Super::BeginPlay();

	auto mesh = GetStaticMeshComponent()->GetStaticMesh();
	if (mesh)
	{
		shotOriginSocket = mesh->FindSocket(FName("ProjectileSpawn"));

		if (!shotOriginSocket)
		{
			UE_LOG(DebugLog, Warning, TEXT("Gun Mesh does not have a 'ProjectileSpawn' socket, shots will originate from incorrect position"));
		}
	}
}

void AHitScanGunActor::OnUsed()
{
	if (bApplyingRecoil) // Gun hasn't reset yet.
		return;

	shoot();
	bApplyingRecoil = true;
	shotAudioComponent->Play();
}

void AHitScanGunActor::OnGrip(UGripMotionControllerComponent* GrippingController, const FBPActorGripInformation& GripInformation) 
{
	grippingController = GrippingController;
	gripInfo = GripInformation;
}

void AHitScanGunActor::OnGripRelease(UGripMotionControllerComponent* ReleasingController, const FBPActorGripInformation& GripInformation) 
{
	grippingController = nullptr;
}

void AHitScanGunActor::shoot() 
{
	FTransform trafo;
	shotOriginSocket->GetSocketTransform(trafo, GetStaticMeshComponent());

	auto forward = trafo.GetRotation().GetForwardVector();
	forward.Normalize();
	auto start = trafo.GetLocation();
	auto end = start + forward * shotRange; 
	
	const TArray<TEnumAsByte<EObjectTypeQuery>> queryTypes{
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic),
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic),
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody),
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn)
	};

	FHitResult result;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	auto* world = GetWorld();
	
	world->LineTraceSingleByObjectType(result, start, end, queryTypes, CollisionParams);
	DrawDebugLine(world, start, end, FColor::Red);

	if (result.bBlockingHit)
	{
		UGameplayStatics::ApplyDamage(result.GetActor(), damage, nullptr, this, UPlayerDamage::StaticClass());
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("shot"));
	}
}

namespace
{
	float animTime = 0.1f;
	float elapsedAnimTime = 0.f;
	float recoilOrigin = 0.f;
	float recoilTarget = -30.f;
	TEnumAsByte<EBPVRResultSwitch::Type> result;
}

void AHitScanGunActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bApplyingRecoil)
	{
		elapsedAnimTime += DeltaTime;

		auto recoil = FMath::Lerp(FVector(0, recoilOrigin, 0), FVector(0, recoilTarget, 0), elapsedAnimTime / animTime);

		FTransform addTrafo;
		addTrafo.AddToTranslation(recoil);

		grippingController->SetGripAdditionTransform(gripInfo, result, addTrafo, true);

		if (elapsedAnimTime >= animTime)
		{
			elapsedAnimTime = 0.f;

			if (recoilTarget < 0) // Gun finished moving back
			{
				recoilOrigin = recoilTarget;
				recoilTarget = 0.f;
			}
			else // Gun is back at original position 
			{
				recoilTarget = recoilOrigin;
				recoilOrigin = 0.f;
				bApplyingRecoil = false;
			}
		}
	}
}






