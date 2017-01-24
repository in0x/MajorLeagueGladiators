#include "Prototype.h"

#include "ActorSpawn.h"


AActorSpawn::AActorSpawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, spawnRadius(0.0f)
	, initialSpawnTime(0.2f)
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
	
	auto rotation = randomStream.GetUnitVector().Rotation();
	rotation.Pitch = 0;
	rotation.Roll = 0;
	
	auto* actor = world->SpawnActor<AActor>(actorToSpawn,
		location,
		rotation	
		);

	if (!actor)
	{
		return;
	}

	actor->SetReplicates(true);
	actor->SetReplicateMovement(true);
}

FVector AActorSpawn::generateRandomSpawnLocation()
{
	const float horizontalDistance = spawnRadius * randomStream.GetFraction();
	FVector randVec = randomStream.GetUnitVector() * horizontalDistance;
	randVec.Z = 0;

	return RootComponent->GetComponentLocation() + randVec;		
}