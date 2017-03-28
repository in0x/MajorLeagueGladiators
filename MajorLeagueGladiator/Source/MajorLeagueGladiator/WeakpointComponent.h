// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "Weakpoint.h"
#include "WeakpointComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAJORLEAGUEGLADIATOR_API UWeakpointComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWeakpointComponent();
	virtual void BeginPlay() override;	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	FWeakpoint FindHitWeakpoint(const FHitResult& Hit) const;

	UPROPERTY(EditAnywhere)
	bool bDebugDrawWeakpoints;

private:
	FVector NameToLocation(const FName& name) const;

	UPROPERTY(EditAnywhere)
	TArray<FWeakpoint> weakpoints;

	USkeletalMeshComponent* skeleton;
};
