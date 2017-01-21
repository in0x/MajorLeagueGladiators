// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/StaticMeshComponent.h"
#include "TriggerZoneComponent.generated.h"

UENUM()
enum class TriggerType : uint8
{
	None,
	Health,
	Ammo
};

UCLASS(meta = (BlueprintSpawnableComponent))
class PROTOTYPE_API UTriggerZoneComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
	
public:
	UTriggerZoneComponent();
	void BeginPlay();

	TriggerType GetTriggerType();
	void SetTriggerType(TriggerType type);

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* SelfComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIdx, bool bFromSweep, const FHitResult& Hit);

	UPROPERTY(EditAnywhere)
	TriggerType triggerType;
};
