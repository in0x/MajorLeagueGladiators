// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MlgGrippableStaticMeshActor.h"
#include "Sword.generated.h"

UCLASS()
class MAJORLEAGUEGLADIATOR_API ASword : public AMlgGrippableStaticMeshActor
{
	GENERATED_BODY()	
public:
	ASword(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

private:
	FVector oldSwingSpeed;

	UPROPERTY(EditAnywhere)
	float damageAppliedOnHit;

	UPROPERTY(EditAnywhere)
	int threshholdDoDamageSquared;

	UPROPERTY(EditAnywhere, Category = "Damage")
	TSubclassOf<UDamageType> damageType;

	//TODO: change sword color changing. THIS IS ONLY TEMPORARY
	UPROPERTY(EditAnywhere)
	UMaterial* materialRedObject;

	UPROPERTY(EditAnywhere)
	UMaterial* materialObject;

	UPROPERTY(EditAnywhere)
	UMaterialInstanceDynamic* materialRedObject_Dyn;

	UPROPERTY(EditAnywhere)
	UMaterialInstanceDynamic* materialObject_Dyn;

	// between 0 and 1
	// defines how fast new sword speed influences overall speedvalue
	UPROPERTY(EditAnywhere)
	float slashVelocityLearnRate;

	void onStartSlash();
	void onEndSlash();
	void damageAllOverlappingActors();

	void setMaterialOfOwnerMesh(UMaterialInstanceDynamic* material_Dyn);

	bool isSwordFastEnough;

	void tryLaunchCharacter(ACharacter* character) const;
	void doRumbleRight();
	bool canDealDamageTo(const ACharacter* OtherCharacter) const;
	void dealDamageTo(ACharacter* OtherCharacter, const FHitResult& HitResult);
	FHitResult getOverlappingHit();
	void getOverlappingHits(TMap<AActor*, FHitResult>& outActorToHit);

	FVector calcRelativeVelocity() const;
};
