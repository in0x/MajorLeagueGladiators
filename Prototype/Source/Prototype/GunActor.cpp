// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "GunActor.h"
#include "AmmoComponent.h"
#include "GunProjectile.h"
#include "TextWidget.h"
#include "WidgetComponent.h"

AGunActor::AGunActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	ammoComponent = ObjectInitializer.CreateDefaultSubobject<UAmmoComponent>(this, TEXT("AmmoComponent"));
	ammoCountWidget = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("AmmoCounterWidget"));
	ammoCountWidget->SetupAttachment(GetStaticMeshComponent(), FName(TEXT("UI"), EFindName::FNAME_Find));
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

	if (bShooting && ammoComponent->ConsumeAmmo())
	{
		FTransform trafo;
		projectileSpawnSocket->GetSocketTransform(trafo, GetStaticMeshComponent());

		auto projectile = GetWorld()->SpawnActor<AGunProjectile>(ammoComponent->GetProjectileType(), trafo);
		checkf(projectile, TEXT("AGunProjectile spawned by AGunActor was null"));
		projectile->GetStaticMeshComponent()->AddImpulse(GetActorRightVector() * projectileVelAccel);
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




