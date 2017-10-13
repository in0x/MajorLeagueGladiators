// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MlgPlayerCharacter.h"
#include "MeleePlayerCharacter.generated.h"

class UAbilityWidgetComponent;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AMeleePlayerCharacter : public AMlgPlayerCharacter
{
	GENERATED_BODY()

public:
	AMeleePlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	UMaterialInterface* swordMaterial;

private:
	virtual void ToggleMenuState(bool bMenuEnabled) override;
	
	void OnLeftTouchpadX(float Value);
	void OnLeftTouchpadY(float Value);
	
	UPROPERTY(EditAnywhere)
	UAbilityWidgetComponent* topAbilityWidget;

	UPROPERTY(EditAnywhere)
	UAbilityWidgetComponent* bottomAbilityWidget;
};
