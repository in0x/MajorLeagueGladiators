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
#include "MlgGameplayStatics.h"
#include "DamageFeedbackComponent.h"

namespace
{
	const char* PAWN_COLLISION_PROFILE_NAME = "Pawn";
}



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
	damageCauser->DamageType = UEnemyDamage::StaticClass();

	damageReciever = ObjectInitializer.CreateDefaultSubobject<UDamageReceiverComponent>(this, TEXT("DamageReciever"));
	//damageVisualizer = ObjectInitializer.CreateDefaultSubobject<UDamageVisualizerComponent>(this, TEXT("DamageVisualizer"));

	GetMesh()->SetCollisionProfileName(PAWN_COLLISION_PROFILE_NAME);
	GetMesh()->bGenerateOverlapEvents = true;
}

void AMlgAICharacter::ApplyStagger_Implementation(float DurationSeconds)
{
	//damageVisualizer = ObjectInitializer.CreateDefaultSubobject<UDamageVisualizerComponent>(this, TEXT("DamageVisualizer")); //TODO: Remove
}

float AMlgAICharacter::InternalTakePointDamage(float Damage, const FPointDamageEvent& PointDamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Damage = Super::InternalTakePointDamage(Damage, PointDamageEvent, EventInstigator, DamageCauser);

	FWeakpoint hitWeakpoint = weakpoints->FindHitWeakpoint(PointDamageEvent.HitInfo);

	UMeshComponent* mesh = FindComponentByClass<UMeshComponent>();
	UDamageFeedbackComponent* damageFeedback = FindComponentByClass<UDamageFeedbackComponent>(); //concrete damage feedback component is created in respective AI characters BP

	if (damageFeedback)
	{
		damageFeedback->DoMaterialVisualization_NetMulticast(mesh);

		if (hitWeakpoint.LocationSocketName != NAME_None)
		{
			//damageVisualizer->AddVisual_NetMulticast(mesh, true, FTransform(PointDamageEvent.HitInfo.ImpactPoint));
			damageFeedback->DoParticleSystemVisualization_NetMulticast(PointDamageEvent.HitInfo.ImpactPoint, PointDamageEvent.ShotDirection, PointDamageEvent.DamageTypeClass);
			damageFeedback->DoWeakpointParticleSystemVisualization_NetMulticast(PointDamageEvent.HitInfo.ImpactPoint, PointDamageEvent.ShotDirection, PointDamageEvent.DamageTypeClass);
			//damageFeedback->PlaySound_NetMulticast()
		}
		else
		{
			//damageVisualizer->AddVisual_NetMulticast(mesh, false);
			damageFeedback->DoParticleSystemVisualization_NetMulticast(PointDamageEvent.HitInfo.ImpactPoint, PointDamageEvent.ShotDirection, PointDamageEvent.DamageTypeClass);
		}
	}
	

	return Damage * hitWeakpoint.DamageMultiplier;
}



