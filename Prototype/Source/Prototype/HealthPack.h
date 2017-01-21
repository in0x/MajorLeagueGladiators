// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UsableItem.h"
#include "MessageEndpoint.h"
#include "Usable.h"
#include "HealthPack.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE_API AHealthPack : public AUsableItem, public IUsable
{
	GENERATED_BODY()
	
public:
	AHealthPack();
	void BeginPlay();

	UFUNCTION()
	virtual void IUse(AActor* user, TriggerType triggerType) override;

private:
	virtual void Use(AActor* CollidingActor, UTriggerZoneComponent* trigger) override;

	UPROPERTY(EditAnywhere)
	uint32 amountToRefill;

	FMessageEndpointPtr msgEndpoint;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* SelfComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIdx, bool bFromSweep, const FHitResult& Hit);
};
