// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "IPlayerCharacterMotionController.h"

#include "HandMotionController.generated.h"

class APlayerCharacter;

/*
* Used to be able to use our mock HandComponents with IModularFeature. This way we don't need
* to change the GripMotionController, as it's going to use the position from out HandComponents
* when no actual MotionController are attached.
*/
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROTOTYPE_API UHandMotionController : public UObject, public IPlayerCharacterMotionController
{
	GENERATED_BODY()

public:
	UHandMotionController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual bool GetControllerOrientationAndPosition(int32 controllerIndex, EControllerHand deviceHand, FRotator& outOrientation, FVector& outPosition) const override;
	virtual ETrackingStatus GetControllerTrackingStatus(int32 controllerIndex, EControllerHand deviceHand) const override;
	virtual void SetPlayerCharacter(APlayerCharacter* playerCharacter) override;

private:
	UPROPERTY()
	APlayerCharacter* playerChar;
};
