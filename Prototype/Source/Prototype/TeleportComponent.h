// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include <tuple>

#include "TeleportComponent.generated.h"

struct TeleportData
{
	FVector Position;
	bool ShouldTeleport;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOTYPE_API UTeleportComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTeleportComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
	
	void Enable(USceneComponent* teleportOrigin);
	TeleportData GetTeleportData();
	void Disable();

private:
	USceneComponent* origin;
	USplineMeshComponent* tpSpline;
	TArray<FVector> positions;
	FVector lastTraceDest;
	TArray<TEnumAsByte<EObjectTypeQuery>> queryTypes{ EObjectTypeQuery::ObjectTypeQuery1 }; // ObjectTypeQuery1 is static world objects
	FHitResult tpHitResult;
	bool shouldTeleport;
};
