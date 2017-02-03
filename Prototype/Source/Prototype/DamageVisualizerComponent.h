// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "DamageVisualizerComponent.generated.h"

class UDamageReceiverComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOTYPE_API UDamageVisualizerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UDamageVisualizerComponent();

	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere, Category = "Visualizer")
	UParticleSystem* particleSystem;

	UPROPERTY(EditAnywhere, Category = "Visualizer")
	UStaticMeshComponent* targetMeshComponent;

	UPROPERTY(EditAnywhere, Category = "Visualizer")
	float materialVisualizationDuration;

	UPROPERTY(EditAnywhere, Category = "Visualizer")
		float particleSystemVisualizationDuration;

	UDamageReceiverComponent* damageReceiver;

	TArray<UParticleSystemComponent*> particleSystemInstances;

	UMaterialInstanceDynamic* matInstance;

	FTimerHandle spawnTimerHandle; //maybe optional?

	UFUNCTION()
	void onDamageReceived(AActor* DamagedActor);

	UFUNCTION()
	void onPointDamageReceived(AActor* DamagedActor, const FVector& HitLocation);
	
	void startMaterialVisualization();

	void startParticleSystemVisualization(const FTransform& visualizationOrigin);

	void stopMaterialVisualization();

	void stopParticleSystemVisualization();
};
