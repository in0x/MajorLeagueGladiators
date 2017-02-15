// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MlgGrippableStaticMeshActor.h"
#include "Usable.h"
#include "UsableItem.generated.h"

UCLASS()
class MAJORLEAGUEGLADIATOR_API AUsableItem : public AMlgGrippableStaticMeshActor, public IUsable
{
	GENERATED_BODY()
	
public:	
	AUsableItem();

	UFUNCTION()
	virtual void Use(AActor* User, TriggerType Type) override;
};
