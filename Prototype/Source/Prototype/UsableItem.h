// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRExpansion/GrippableStaticMeshActor.h"
#include "UsableItem.generated.h"

class UTriggerZoneComponent;

UCLASS()
class PROTOTYPE_API AUsableItem : public AGrippableStaticMeshActor
{
	GENERATED_BODY()
	
public:	
	AUsableItem();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* SelfComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIdx, bool bFromSweep, const FHitResult& Hit);

private:
	// Called when the item collides with one of it's trigger zones.
	virtual void Use(AActor* ColidingActor, UTriggerZoneComponent* trigger);
};
