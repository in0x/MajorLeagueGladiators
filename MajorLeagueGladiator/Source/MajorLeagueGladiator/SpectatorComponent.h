// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SpectatorComponent.generated.h"

UCLASS()
class MAJORLEAGUEGLADIATOR_API USpectatorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpectatorComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	void SetSceneCapture(USceneCaptureComponent2D* SceneCapture);
	void Create();

private:
	TSharedPtr<SWindow> window;

	UPROPERTY(EditAnywhere)
	FVector2D windowSize;

	UPROPERTY()
	UImage* img;

	UPROPERTY()
	USceneCaptureComponent2D* sceneCapture;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* widgetMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* matInstance;

	UFUNCTION()
	void EndPlayCallback(AActor* Actor, EEndPlayReason::Type EndPlayReason);
};