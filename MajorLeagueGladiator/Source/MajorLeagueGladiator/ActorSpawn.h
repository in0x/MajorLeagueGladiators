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
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	bool HasSpawnLimit() const;
	bool IsAllowedToSpawn() const;

private:
	UFUNCTION()
	void spawnActor();

	UFUNCTION()
	void OnSpawnedActorEndPlay(AActor* Actor, EEndPlayReason::Type EndPlayReason);

	FRotator generateRandomRotator();
	FVector generateRandomSpawnLocation();
	float generateRandomSpawnTime();

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> actorToSpawn;

	UPROPERTY(EditAnywhere)
	float spawnRadius;

	UPROPERTY(EditAnywhere)
	int32 randomSeed;

	UPROPERTY(EditAnywhere)
	float minSpawnTime;

	UPROPERTY(EditAnywhere)
	float maxSpawnTime;

	/*Max amount of spawned actors that can be alive. 0 Stands for infinite*/
	UPROPERTY(EditAnywhere)
	int32 maxSpawnActors;

	UPROPERTY()
	TArray<AActor*> spawnedActors;

	int32 currentlySpawnedActors;

	FRandomStream randomStream;

	FTimerHandle spawnTimerHandle;
};