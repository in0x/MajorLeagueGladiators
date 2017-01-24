#include "Prototype.h"

#include "ActorSpawn.h"


AActorSpawn::AActorSpawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, initialSpawnTime(.2f)
{	
	SetRootComponent(ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootComponet")));
	randomStream.Initialize(randomSeed);
}

void AActorSpawn::SetRespawnTimer(float IntervalInSeconds)
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	FTimerManager& timer = GetWorldTimerManager();
	timer.SetTimer(spawnTimerHandle, this, &AActorSpawn::spawnActor, IntervalInSeconds, true);
}

void AActorSpawn::BeginPlay()
{
	Super::BeginPlay();
	SetRespawnTimer(initialSpawnTime);
}

void AActorSpawn::spawnActor()
{
	UWorld* world = GetWorld();
	auto location = generateRandomSpawnLocation();
	auto* actor = world->SpawnActor<AActor>(actorToSpawn,
		location,
		randomStream.GetUnitVector().Rotation()
		);

	actor->SetReplicates(true);
	actor->SetReplicateMovement(true);
}

FVector AActorSpawn::generateRandomSpawnLocation()
{
	float distance = spawnRadius * randomStream.GetFraction();

	return RootComponent->GetComponentLocation() + randomStream.GetUnitVector() * distance;
}