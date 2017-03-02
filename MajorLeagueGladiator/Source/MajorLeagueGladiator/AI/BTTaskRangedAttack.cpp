// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "BTTaskRangedAttack.h"
#include "AIController.h"
#include "Projectiles/PhysicsProjectile.h"

#include "BehaviorTree/BTFunctionLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTTaskRangedAttack::UBTTaskRangedAttack()
	: ProjectileClass(APhysicsProjectile::StaticClass())
	, SocketName("gunSocket")
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
	APawn* pawn = controller->GetPawn();
	USkeletalMeshComponent* meshComponent = pawn->FindComponentByClass<USkeletalMeshComponent>();

	FTransform socketTransform = meshComponent->GetSocketTransform(SocketName);	

	AActor* targetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(Target.SelectedKeyName));

	FVector targetDir = targetActor->GetActorLocation() - socketTransform.GetLocation();
	targetDir.Normalize();

	ProjectileClass.GetDefaultObject()->FireProjectile(socketTransform.GetLocation(), targetDir, pawn, controller);

	return EBTNodeResult::Succeeded;
}
