// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "PlayerHudWidget.generated.h"

class APlayerCharacter;

UCLASS()
class PROTOTYPE_API UPlayerHudWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void OnAttachPlayer(APlayerCharacter* PlayerCharacter);

protected:
	UPROPERTY(EditAnywhere, Category = "Hud")
	float healthPercentage = 1;

private:
	UFUNCTION()
	void setCurrentHealthPercentage(float percentage);
};