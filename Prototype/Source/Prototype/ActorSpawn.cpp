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
	world->SpawnActor<AActor>(actorToSpawn,
		generateRandomSpawnLocation(),
		randomStream.GetUnitVector().Rotation()
		);
}

FVector AActorSpawn::generateRandomSpawnLocation()
{
	return RootComponent->GetComponentLocation() + randomStream.GetUnitVector() * spawnRadius;
}