// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "DamageFeedbackComponent.generated.h"

USTRUCT()
struct FDamageFeedbackData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "DamageVisual")
	UMeshComponent* AffectedMesh;

	UPROPERTY(EditAnywhere, Category = "DamageVisual")
	UMaterialInstanceDynamic* MatInstance;

	UPROPERTY(EditAnywhere, Category = "DamageVisual")
	FName DamageValParamName;

	UPROPERTY(EditAnywhere, Category = "DamageVisual")
	float MatVisDuration;

	UPROPERTY(EditAnywhere, Category = "DamageVisual")
	float CurrentMatVisDuration;

	FDamageFeedbackData()
		: AffectedMesh(nullptr)
		, MatInstance(nullptr)
		, DamageValParamName(FName("DamageValue"))
		, MatVisDuration(0.3f)
		, CurrentMatVisDuration(0.f)
	{}
};

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAJORLEAGUEGLADIATOR_API UDamageFeedbackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UDamageFeedbackComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(NetMulticast, reliable)
	virtual void DoMaterialVisualization_NetMulticast(UMeshComponent* AffectedMesh);

	UFUNCTION(NetMulticast, reliable)
	virtual void DoParticleSystemVisualization_NetMulticast(const FVector& HitLocation, const FVector& OriginDirection, TSubclassOf<UDamageType> DamageType = nullptr);

	UFUNCTION(NetMulticast, reliable)
	virtual void DoWeakpointParticleSystemVisualization_NetMulticast(const FVector& HitLocation, const FVector& OriginDirection, TSubclassOf<UDamageType> DamageType = nullptr);

	UFUNCTION(NetMulticast, reliable)
	virtual void PlaySound_NetMulticast(const FVector& location, const FVector& OriginDirection, const UDamageType* DamageType = nullptr);
		
protected:

	TArray<FDamageFeedbackData> feedbackData;

	UPROPERTY(EditAnywhere, Category = "Feedback")
	TArray<UParticleSystem*> damageParticleSystems;

	UPROPERTY(EditAnywhere, Category = "Feedback")
	TArray<UParticleSystem*> weakpointParticleSystems;

	UPROPERTY(EditAnywhere, Category = "Feedback")
	TArray<USoundBase*> sounds;

	//weakpointSounds???
};
