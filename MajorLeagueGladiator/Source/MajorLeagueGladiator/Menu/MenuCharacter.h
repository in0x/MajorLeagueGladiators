// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRExpansion/VRSimpleCharacter.h"
#include "MenuCharacter.generated.h"

/**
 * 
 */
UCLASS()
class MAJORLEAGUEGLADIATOR_API AMenuCharacter : public AVRSimpleCharacter
{
	GENERATED_BODY()
public:
	AMenuCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
private:
	UPROPERTY(EditAnywhere, Category = "Mesh")
	UStaticMeshComponent* leftMesh;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	UStaticMeshComponent* rightMesh;

	void OnLeftTriggerClicked();
	void OnRightTriggerClicked();
	
	
};
