// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRExpansion/VRSimpleCharacter.h"
#include "PlayerCharacter.generated.h"

/**
 *
 */
UCLASS()
class PROTOTYPE_API APlayerCharacter : public AVRSimpleCharacter
{
	GENERATED_BODY()
public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* playerInputComponent) override;
	virtual void Tick(float DeltaTime) override;

	void MoveForward(float value);
	void MoveRight(float value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* leftMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* rightMesh;

	//UPROPERTY(VisibleAnywhere)
	//USceneComponent* leftHand; // GripMotionController || "HandComponent"

	//UPROPERTY(VisibleAnywhere)
	//USceneComponent* rightHand;
};
