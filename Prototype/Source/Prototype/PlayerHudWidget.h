// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "PlayerHudWidget.generated.h"

/**
 * 
 */
class APlayerCharacter;

UCLASS()
class PROTOTYPE_API UPlayerHudWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hud")
		float healthPercentage = 1;
	
	void OnAttachPlayer(APlayerCharacter* PlayerCharacter);


private:
	UFUNCTION()
	void setCurrentHealthPercentage(float percentage);
};