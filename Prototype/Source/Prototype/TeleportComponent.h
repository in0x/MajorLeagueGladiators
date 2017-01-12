// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "TeleportComponent.generated.h"

struct TeleportResult
{
	FVector Position;
	bool ShouldTeleport;
};

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class PROTOTYPE_API UTeleportComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

public:	
	UTeleportComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
	
	void Enable(USceneComponent* TeleportOrigin);
	TeleportResult GetTeleportResult() const;
	void Disable();

private:
	USceneComponent* origin;
	
	// PredictProjectilePath Hit Test Data
	FVector lastTraceDest;
	FHitResult tpHitResult;
	TArray<FVector> positions;
	TArray<TEnumAsByte<EObjectTypeQuery>> queryTypes{ EObjectTypeQuery::ObjectTypeQuery1 }; // ObjectTypeQuery1 is static world objects
	bool shouldTeleport;
};
