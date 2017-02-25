// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "GunActor.h"
#include "AmmoComponent.h"
#include "Projectiles/PhysicsProjectile.h"
#include "TextWidget.h"
#include "WidgetComponent.h"
#include "MlgPlayerController.h"

AGunActor::AGunActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	SetReplicates(true);

	ammoComponent = ObjectInitializer.CreateDefaultSubobject<UAmmoComponent>(this, TEXT("AmmoComponent"));
	
	ammoCountWidget = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("AmmoCounterWidget"));
	ammoCountWidget->SetupAttachment(GetStaticMeshComponent(), FName(TEXT("UI"), EFindName::FNAME_Find));
	ammoCountWidget->SetIsReplicated(true);
}

void AGunActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}	

void AGunActor::BeginPlay()
{
	Super::BeginPlay();
	
	auto mesh = GetStaticMeshComponent()->GetStaticMesh();
	if (mesh)
	{
		projectileSpawnSocket = mesh->FindSocket(FName("ProjectileSpawn"));

		if (!projectileSpawnSocket)
		{
			UE_LOG(DebugLog, Warning, TEXT("Gun Mesh does not have a 'ProjectileSpawn' socket, projectiles will be spawned at incorrect position"));
		}
	}

	textWidget = CastChecked<UTextWidget>(ammoCountWidget->GetUserWidgetObject());
	textWidget->SetText(FString::FromInt(ammoComponent->GetAmmoCount()));
	
	ammoComponent->OnAmmoChanged.AddLambda([this](int32 newCount) 
	{
		textWidget->SetText(FString::FromInt(newCount));
	});
	
	SetActorTickInterval(shotFrequency);
}

void AGunActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShooting && ammoComponent->GetAmmoCount() > 0)
	{
		ammoComponent->ConsumeAmmo(); 
	
		FActorSpawnParameters spawnParameters;
		spawnParameters.Owner = this;
		spawnParameters.Instigator = GetMlgPlayerController()->GetPawn();

		FTransform trafo;
		projectileSpawnSocket->GetSocketTransform(trafo, GetStaticMeshComponent());

		auto projectile = GetWorld()->SpawnActor<AGunProjectile>(ammoComponent->GetProjectileType(), trafo, spawnParameters);
		checkf(projectile, TEXT("AGunProjectile spawned by AGunActor was null"));
		projectile->GetStaticMeshComponent()->AddImpulse(trafo.GetRotation().GetForwardVector() * projectileVelAccel);

		auto* controller = this->GetMlgPlayerController();
		if (controller != nullptr) 
		{
			controller->ClientPlayForceFeedback(controller->GetRumbleShortRight(), false, FName("rumbleRight"));
		}
	}
}

void AGunActor::OnUsed()
{
	bShooting = true;
}

void AGunActor::OnEndUsed()
{
	bShooting = false;
}




