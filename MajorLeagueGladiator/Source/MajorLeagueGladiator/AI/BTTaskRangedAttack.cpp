// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "BTTaskRangedAttack.h"
#include "AIController.h"
#include "Projectiles/PhysicsProjectile.h"

#include "BehaviorTree/BTFunctionLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MlgGameplayStatics.h"

#include "ShootPhysicsProjectileComponent.h"

// "gunSocket"

UBTTaskRangedAttack::UBTTaskRangedAttack()
	: controller(nullptr)
{
	NodeName = "BTTaskRangedAttack";
	bCreateNodeInstance = true;
}

void UBTTaskRangedAttack::SetOwner(AActor* ActorOwner)
{
	controller = Cast<AAIController>(ActorOwner);
}

EBTNodeResult::Type UBTTaskRangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AActor* targetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(Target.SelectedKeyName));
	if (targetActor == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	APawn* pawn = controller->GetPawn();

	pawn->FindComponentByClass<UShootPhysicsProjectileComponent>()->ShootAt(targetActor->GetActorLocation());

	return EBTNodeResult::Succeeded;
}
