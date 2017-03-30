// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgAICharacter.h"
#include "HealthComponent.h"
#include "WeakpointComponent.h"
#include "TriggerZoneComponent.h"
#include "DamageCauserComponent.h"
#include "DamageTypes/EnemyDamage.h"
#include "DamageReceiverComponent.h"
#include "DamageVisualizerComponent.h"

AMlgAICharacter::AMlgAICharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	health = ObjectInitializer.CreateDefaultSubobject<UHealthComponent>(this, TEXT("Health"));
	weakpoints = ObjectInitializer.CreateDefaultSubobject<UWeakpointComponent>(this, TEXT("Weakpoints"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> cubeMesh(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	triggerZone = ObjectInitializer.CreateDefaultSubobject<UTriggerZoneComponent>(this, TEXT("TriggerZone"));
	triggerZone->SetTriggerType(TriggerType::Health);
	triggerZone->SetupAttachment(GetRootComponent());
	triggerZone->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	triggerZone->SetRelativeLocation(FVector(0.f, 0.f, -70.f));
	triggerZone->SetStaticMesh(cubeMesh.Object);

	damageCauser = ObjectInitializer.CreateDefaultSubobject<UDamageCauserComponent>(this, TEXT("DamageCauser"));
	damageCauser->SetDamageType(UEnemyDamage::StaticClass());

	damageReciever = ObjectInitializer.CreateDefaultSubobject<UDamageReceiverComponent>(this, TEXT("DamageReciever"));
	damageVisualizer = ObjectInitializer.CreateDefaultSubobject<UDamageVisualizerComponent>(this, TEXT("DamageVisualizer"));
}

float AMlgAICharacter::InternalTakePointDamage(float Damage, const FPointDamageEvent& PointDamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	FWeakpoint hitWeakpoint = weakpoints->FindHitWeakpoint(PointDamageEvent.HitInfo);

	UMeshComponent* mesh = FindComponentByClass<UMeshComponent>();

	if (hitWeakpoint.LocationSocketName != NAME_None)
	{
		damageVisualizer->AddVisual_NetMulticast(mesh, true, FTransform(PointDamageEvent.HitInfo.ImpactPoint));
	}
	else
	{
		damageVisualizer->AddVisual_NetMulticast(mesh, false);
	}

	return Damage * hitWeakpoint.DamageMultiplier;
}


