#pragma once

#include "ActorSpawn.generated.h"

UCLASS()
class AActorSpawn : public AActor
{
	GENERATED_BODY()

public:
	AActorSpawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void SetRespawnTimer(float IntervalInSeconds);

	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void spawnActor();

	FRotator generateRandomRotator();
	FVector generateRandomSpawnLocation();

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> actorToSpawn;

	UPROPERTY(EditAnywhere)
	float spawnRadius;

	UPROPERTY(EditAnywhere)
	int32 randomSeed;

	UPROPERTY(EditAnywhere)
	float initialSpawnTime;	

	FRandomStream randomStream;

	FTimerHandle spawnTimerHandle;
};