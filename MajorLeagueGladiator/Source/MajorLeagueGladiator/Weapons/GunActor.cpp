// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "GunActor.h"
#include "AmmoComponent.h"
#include "Projectiles/PhysicsProjectile.h"
#include "TextWidget.h"
#include "WidgetComponent.h"
#include "Characters/MlgPlayerCharacter.h"

namespace
{
	const FName PROJECTILE_SPAWN_SOCKET_NAME("ProjectileSpawn");
}

AGunActor::AGunActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	SetReplicates(true);

	ammoComponent = ObjectInitializer.CreateDefaultSubobject<UAmmoComponent>(this, TEXT("AmmoComponent"));
	
	ammoCountWidget = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("AmmoCounterWidget"));
	ammoCountWidget->SetupAttachment(MeshComponent, FName(TEXT("UI"), EFindName::FNAME_Find));
	ammoCountWidget->SetIsReplicated(true);
}

void AGunActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}	

void AGunActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (!(MeshComponent->DoesSocketExist(PROJECTILE_SPAWN_SOCKET_NAME)))
	{
		UE_LOG(DebugLog, Warning, TEXT("HitscanGun Mesh does not have a 'ProjectileSpawn' socket, shots will originate from incorrect position"));
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

	if (bShooting && ammoComponent->GetAmmoCount() > 0 && HasAuthority())
	{
		ammoComponent->ConsumeAmmo(); 

		FTransform trafo = MeshComponent->GetSocketTransform(PROJECTILE_SPAWN_SOCKET_NAME);

		ammoComponent->GetProjectileType().GetDefaultObject()->FireProjectile(
			trafo.GetLocation(), trafo.Rotator().Vector() , this, Instigator->GetController());

		AMlgPlayerCharacter* player = CastChecked<AMlgPlayerCharacter>(GetOwner());
		player->PlayRumbleRight();
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




