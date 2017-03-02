// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/StaticMeshComponent.h"
#include "RespawnComponent.generated.h"


/*
NOTE(Phil):
	This is a cobeled together component we use to catch stuff
	falling out of the level when showing of the game. This
	should not make it into a finished version.
*/
UCLASS(meta = (BlueprintSpawnableComponent))
class MAJORLEAGUEGLADIATOR_API URespawnComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	URespawnComponent();
	void BeginPlay();

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* SelfComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIdx, bool bFromSweep, const FHitResult& Hit);

	UPROPERTY(EditAnywhere)
	FVector respawnPosition;
};
