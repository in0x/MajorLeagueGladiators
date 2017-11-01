// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRExpansion/VRSimpleCharacter.h"
#include "TravelMapCharacter.generated.h"

class UWidgetInteractionComponent;
class UUserWidget;

UCLASS()
class MAJORLEAGUEGLADIATOR_API ATravelMapCharacter : public AVRSimpleCharacter
{
	GENERATED_BODY()
public:
	ATravelMapCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void BeginPlay() override;
	
private:	
	UPROPERTY(EditAnywhere)
	UWidgetComponent* loadingWidgetComponent;
};
