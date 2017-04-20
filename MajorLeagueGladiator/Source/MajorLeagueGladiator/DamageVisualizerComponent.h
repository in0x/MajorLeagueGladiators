// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "DamageVisualizerComponent.generated.h"

// On Damage, instead of setting class members, add another FDamageVisual
// In Tick, process them all
// In OnPointDamage, check if we have a SkeletalMesh, if yes, use the skinned submesh

USTRUCT()
struct FDamageVisual
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category="DamageVisual")
	TArray<UParticleSystemComponent*> ParticleSystems;

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

	FDamageVisual()
		: AffectedMesh(nullptr)
		, MatInstance(nullptr)
		, DamageValParamName(FName("DamageValue"))
		, MatVisDuration(0.3f)
		, CurrentMatVisDuration(0.f)
	{}
};

	


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAJORLEAGUEGLADIATOR_API UDamageVisualizerComponent : public UActorComponent
{
	GENERATED_BODY()


public:	
	UDamageVisualizerComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(NetMulticast, reliable)
	void AddVisual_NetMulticast(UMeshComponent* affectedMesh, bool bSpawnParticles, const FTransform& particleTrafo = FTransform(), const UDamageType* DamageType = nullptr);

private:
	TArray<FDamageVisual> visuals;
	
	UPROPERTY(EditAnywhere, Category = "Feedback")
	TArray<UParticleSystem*> hitscanProjectileDamageParticleSystems;

	UPROPERTY(EditAnywhere, Category = "Feedback")
	TArray<UParticleSystem*> swordDamageParticleSystems;
};
