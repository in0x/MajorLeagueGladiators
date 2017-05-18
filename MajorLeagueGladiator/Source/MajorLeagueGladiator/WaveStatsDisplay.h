// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "WaveStatsDisplay.generated.h"

class UWidgetComponent;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AWaveStatsDisplay : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWaveStatsDisplay(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//	UWidgetComponent* widgetComponent;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

public:	

	UFUNCTION(BlueprintNativeEvent)
	void UpdateWaveNumber(int NewWaveCount);

	UFUNCTION(BlueprintNativeEvent)
	void UpdateEnemyCount(int NewEnemyCount);
private:

	

	void OnWaveNumberChanged(int NewWaveCount);
	void OnEnemyCountChanged(int NewCount, int OldCount);
	
};
