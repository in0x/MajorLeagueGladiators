// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MlgPlayerCharacter.h"
#include "RangedPlayerCharacter.generated.h"

class UAbilityWidgetComponent;

UCLASS()
class MAJORLEAGUEGLADIATOR_API ARangedPlayerCharacter : public AMlgPlayerCharacter
{
	GENERATED_BODY()
	
public:
	ARangedPlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;

private:
	void OnLeftTouchpadX(float Value);
	void OnLeftTouchpadY(float Value);

	UPROPERTY(EditAnywhere)
	UAbilityWidgetComponent* topAbilityWidget;

	UPROPERTY(EditAnywhere)
	UAbilityWidgetComponent* bottomAbilityWidget;
};
