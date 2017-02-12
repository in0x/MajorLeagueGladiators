// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "HitScanGunActor.h"
#include "DamageTypes/PlayerDamage.h"

AHitScanGunActor::AHitScanGunActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, recoilAnimBackDuration(0.5f)
	, recoilAnimForwardDuration(0.5f)
	, elapsedAnimTime(0.f)
	, recoilOrigin(0.f)
	, recoilDistance(-30.f)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	SetReplicates(true);
	bReplicateMovement = true;
	bStaticMeshReplicateMovement = true;

	GetStaticMeshComponent()->SetIsReplicated(true);

	shotAudioComponent = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("ShotAudioComponent"));
	shotAudioComponent->SetIsReplicated(true);

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

	playShotEffect_NetMulticast();
}

void AHitScanGunActor::playShotEffect_NetMulticast_Implementation()
{
	currentAnimDuration = recoilAnimBackDuration;
	bApplyingRecoil = true;
	shotAudioComponent->Play();
}

void AHitScanGunActor::OnGrip(UGripMotionControllerComponent* GrippingController, const FBPActorGripInformation& GripInformation) 
{
	grippingController = GrippingController;
	gripInfo = GripInformation;
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
	
	if (result.bBlockingHit)
	{
		UGameplayStatics::ApplyDamage(result.GetActor(), damage, nullptr, this, UPlayerDamage::StaticClass());
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("shot"));
	}
}

void AHitScanGunActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bApplyingRecoil)
	{
		elapsedAnimTime += DeltaTime;

		auto recoil = FMath::Lerp(FVector(0, recoilOrigin, 0), FVector(0, recoilDistance, 0), elapsedAnimTime / currentAnimDuration);

		TEnumAsByte<EBPVRResultSwitch::Type> result;
		FTransform addTrafo;
		addTrafo.AddToTranslation(recoil);

		grippingController->SetGripAdditionTransform(gripInfo, result, addTrafo, true);

		if (elapsedAnimTime >= currentAnimDuration)
		{
			elapsedAnimTime = 0.f;

			if (recoilDistance < 0) // Gun finished moving back
			{
				recoilOrigin = recoilDistance;
				recoilDistance = 0.f;
				currentAnimDuration = recoilAnimForwardDuration;
			}
			else // Gun is back at original position 
			{
				recoilDistance = recoilOrigin;
				recoilOrigin = 0.f;
				bApplyingRecoil = false;
			}
		}
	}
}






