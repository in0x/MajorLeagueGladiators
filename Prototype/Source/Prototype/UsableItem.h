// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRExpansion/GrippableStaticMeshActor.h"
#include "Usable.h"
#include "UsableItem.generated.h"

UCLASS()
class PROTOTYPE_API AUsableItem : public AGrippableStaticMeshActor, public IUsable
{
	GENERATED_BODY()
	
public:	
	AUsableItem();

	UFUNCTION()
	virtual void Use(AActor* user, TriggerType triggerType) override;
};
