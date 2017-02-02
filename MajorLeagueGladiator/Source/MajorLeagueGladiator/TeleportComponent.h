// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "TeleportComponent.generated.h"

struct TeleportResult
{
	FVector Position;
	bool ShouldTeleport;
};

DECLARE_MULTICAST_DELEGATE_OneParam(CooldownChangeDelegate, float);

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class MAJORLEAGUEGLADIATOR_API UTeleportComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

public:	
	UTeleportComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
	
	void Enable(USceneComponent* TeleportOrigin);
	TeleportResult GetTeleportResult();
	void Disable();

	CooldownChangeDelegate OnCooldownChange;

private:
	UPROPERTY(EditAnywhere)
	float cooldown;
	float elapsedCooldown;
	USceneComponent* origin;	
	
	// PredictProjectilePath Hit Test Data
	FVector lastTraceDest;
	FHitResult tpHitResult;
	TArray<FVector> positions;
	TArray<TEnumAsByte<EObjectTypeQuery>> queryTypes{ EObjectTypeQuery::ObjectTypeQuery1 }; // ObjectTypeQuery1 is static world objects
	bool shouldTeleport;
};
