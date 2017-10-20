// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskRangedAttack.generated.h"

class ABaseProjectile;

/**
 * 
 */
UCLASS()
class MAJORLEAGUEGLADIATOR_API UBTTaskRangedAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTaskRangedAttack();

	virtual void SetOwner(AActor* ActorOwner) override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector Target;

protected:
	UPROPERTY(Transient)
	AAIController* controller;
};
