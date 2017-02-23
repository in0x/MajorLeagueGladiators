// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArcAimComponent.generated.h"

struct ArcAimResult
{
	FVector Position;
	bool bDidAimFindPosition;
};

DECLARE_MULTICAST_DELEGATE_OneParam(CooldownChangeDelegate, float);

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class MAJORLEAGUEGLADIATOR_API UArcAimComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

public:	
	UArcAimComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
	
	void Enable(USceneComponent* TeleportOrigin);
	ArcAimResult GetAimResult();
	void Disable();

	CooldownChangeDelegate OnCooldownChange;

	float GetPredictProjectileForce() const
	{
		return 1000.f;
	}

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
	bool bDidAimFindPosition;
};
