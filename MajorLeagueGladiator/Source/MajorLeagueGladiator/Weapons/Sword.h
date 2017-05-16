// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MlgGrippableMeshActor.h"
#include "Sword.generated.h"

UCLASS()
class MAJORLEAGUEGLADIATOR_API ASword : public AMlgGrippableMeshActor
{
	GENERATED_BODY()	
public:
	ASword(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	void SetIsAlwaysFastEnough(bool IsAlwaysFastEnough);
	bool GetIsAlawaysFastEnough() const;
protected:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

private:
	void onStartSlash();
	void onEndSlash();
	void doRumbleRight();
	void setMaterialOfOwnerMesh(UMaterialInstanceDynamic* material_Dyn);
	void dealDamageTo(ACharacter* OtherCharacter, const FHitResult& HitResult);
	void updateMaterialIntensity(float intensity);
	void updateMaterialColor(const FLinearColor& color);


	void damageAllOverlappingActors();
	void tryLaunchCharacter(ACharacter* character) const;
	bool canDealDamageTo(const ACharacter* OtherCharacter) const;
	void getOverlappingHits(TArray<TPair<AActor*, FHitResult>>& outActorToHit) const;
	void UpdateVelocity(float DeltaTime);
	FVector calcRelativeVelocity() const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere)
	USoundCue* swordSwingCue;

	FVector oldSwingSpeed;

	UPROPERTY(EditAnywhere)
	float damageAppliedOnHit;

	UPROPERTY(EditAnywhere)
	int threshholdDoDamageSquared;

	UPROPERTY(EditAnywhere, Category = "Damage")
	TSubclassOf<UDamageType> damageType;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* materialInstance;

	UPROPERTY(EditAnywhere)
	FLinearColor originalSwordColor;

	UPROPERTY(EditAnywhere)
	FLinearColor damageSwordColor;

	UPROPERTY(EditAnywhere)
	float glowStrengthMultiplier;

	UPROPERTY(EditAnywhere)
	float minGlowStrength;

	UPROPERTY(EditAnywhere)
	float maxGlowStrength;

	// Between 0 and 1 defines how fast new sword speed influences overall speedvalue
	UPROPERTY(EditAnywhere)
	float slashVelocityLearnRate;

	bool bIsSwordFastEnough;

	UPROPERTY(replicated)
	bool bIsAlwaysFastEnough;

	FVector	lastLocation;
	FVector currentVelocity;
};
