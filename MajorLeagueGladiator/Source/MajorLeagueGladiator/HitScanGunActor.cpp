// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "HitScanGunActor.h"
#include "DamageTypes/PlayerDamage.h"
#include "AmmoComponent.h"
#include "WidgetComponent.h"
#include "TextWidget.h"
#include "MlgPlayerController.h"
#include "Projectiles/HitscanProjectile.h"

AHitScanGunActor::AHitScanGunActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, recoilAnimBackDuration(0.5f)
	, recoilAnimForwardDuration(0.5f)
	, elapsedAnimTime(0.f)
	, recoilOrigin(0.f)
	, recoilDistance(-30.f)
	, shotClass(AHitscanProjectile::StaticClass())
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

	/*boltAction = ObjectInitializer.CreateDefaultSubobject<UChildActorComponent>(this, TEXT("BoltAction")); // Removed until we upgrade to 4.15
	boltAction->SetupAttachment(GetRootComponent(), FName("BoltAction"));

	static ConstructorHelpers::FObjectFinder<UBlueprint> BoltActionBP(TEXT("/Game/BluePrints/BoltActionBP"));
	boltAction->SetChildActorClass(*BoltActionBP.Object->GeneratedClass);*/

	sceneCapture = ObjectInitializer.CreateDefaultSubobject<USceneCaptureComponent2D>(this, TEXT("SceneCapture"));
	scopeMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("ScopeMesh"));
	scopeMesh->SetupAttachment(GetRootComponent(), FName("UI"));

	ammoComponent = ObjectInitializer.CreateDefaultSubobject<UAmmoComponent>(this, TEXT("AmmoComponent"));

	ammoCountWidget = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("AmmoCounterWidget"));
	ammoCountWidget->SetupAttachment(GetRootComponent(), FName(TEXT("UI")));
	ammoCountWidget->SetIsReplicated(true);
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
			UE_LOG(DebugLog, Warning, TEXT("HitscanGun Mesh does not have a 'ProjectileSpawn' socket, shots will originate from incorrect position"));
		}
	}

	auto instance = UMaterialInstanceDynamic::Create(scopeMesh->GetMaterial(0), scopeMesh);
	instance->SetTextureParameterValue(FName("ScopeTex"), sceneCapture->TextureTarget);
	scopeMesh->SetMaterial(0, instance);

	textWidget = CastChecked<UTextWidget>(ammoCountWidget->GetUserWidgetObject());
	textWidget->SetText(FString::FromInt(ammoComponent->GetAmmoCount()));

	ammoComponent->OnAmmoChanged.AddLambda([this](int32 newCount)
	{
		textWidget->SetText(FString::FromInt(newCount));
	});
}

void AHitScanGunActor::OnUsed()
{
	if (bIsApplyingRecoil || ammoComponent->GetAmmoCount() <= 0) // Gun hasn't reset yet.
		return;

	ammoComponent->ConsumeAmmo();
	shoot();

	playShotEffect_NetMulticast();

	auto* controller = this->GetMlgPlayerController();
	if (controller != nullptr)
	{
		controller->ClientPlayForceFeedback(controller->GetRumbleShortRight(), false, FName("rumbleRight"));
	}
}

void AHitScanGunActor::playShotEffect_NetMulticast_Implementation()
{
	currentAnimDuration = recoilAnimBackDuration;
	bIsApplyingRecoil = true;
	shotAudioComponent->Play();
}

void AHitScanGunActor::OnGrip(UGripMotionControllerComponent* GrippingController, const FBPActorGripInformation& GripInformation) 
{
	Super::OnGrip(GrippingController, GripInformation);

	grippingController = GrippingController;
	gripInfo = GripInformation;
}

void AHitScanGunActor::shoot() 
{
	FTransform trafo;
	shotOriginSocket->GetSocketTransform(trafo, GetStaticMeshComponent());

	shotClass.GetDefaultObject()->FireProjectile(trafo.GetLocation(), trafo.GetRotation().GetForwardVector(), this, GetMlgPlayerController());
}

void AHitScanGunActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsApplyingRecoil)
	{
		elapsedAnimTime += DeltaTime;

		auto recoil = FMath::Lerp(FVector(0, recoilOrigin, 0), FVector(0, recoilDistance, 0), elapsedAnimTime / currentAnimDuration);

		EBPVRResultSwitch result;
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
				bIsApplyingRecoil = false;
			}
		}
	}
}






