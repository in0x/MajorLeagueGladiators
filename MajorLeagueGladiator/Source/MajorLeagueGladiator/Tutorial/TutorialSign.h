// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "TutorialSign.generated.h"

/**
 * 
 */
UCLASS()
class MAJORLEAGUEGLADIATOR_API ATutorialSign : public AStaticMeshActor
{
	GENERATED_BODY()
public:
	ATutorialSign();
	virtual void BeginPlay() override;
		
private:
	UPROPERTY(EditAnywhere)
	UTexture* signTexture;

	UMaterialInterface* materialInterface;
};
