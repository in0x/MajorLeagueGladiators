// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgAICharacter.h"
#include "HealthComponent.h"
#include "TriggerZoneComponent.h"
#include "DamageCauserComponent.h"
#include "DamageReceiverComponent.h"
#include "DamageVisualizerComponent.h"
#include "DamageTypes/EnemyDamage.h"

AMlgAICharacter::AMlgAICharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	health = ObjectInitializer.CreateDefaultSubobject<UHealthComponent>(this, TEXT("Health"));
	
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
	return Damage;
}


