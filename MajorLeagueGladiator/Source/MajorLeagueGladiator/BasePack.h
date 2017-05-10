// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UsableItem.h"
#include "BasePack.generated.h"

class UPackMovementComponent;
class AMlgPlayerCharacter;

UCLASS(Abstract)
class MAJORLEAGUEGLADIATOR_API ABasePack : public AUsableItem
{
	GENERATED_BODY()

public:		
	ABasePack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;

	void ChargePackIfPossible(const AMlgPlayerCharacter* PlayerCharacter);
	void OnLaunch();

	bool IsCharged() const;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AMlgPlayerCharacter> chargingPlayerClass;

private:
	void chargePack();
	bool canBeChargedBy(const AMlgPlayerCharacter* PlayerCharacter) const;
	void setTimeChargeLeftSeonds(float newValue);
	void updateMaterial();

	UFUNCTION()
	void onCollision(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(NetMulticast, Reliable)
	void chargePack_NetMulticast();

	UFUNCTION(NetMulticast, Reliable)
	void onLaunch_NetMulticast();

	UPROPERTY(EditAnywhere)
	UPackMovementComponent* movementComponent;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* materialInstance;

	/* How Long the Pack should stay charged */
	UPROPERTY(EditAnywhere)
	float maxTimeChargedSeconds;

	float timeChargeLeftSeconds;
};
