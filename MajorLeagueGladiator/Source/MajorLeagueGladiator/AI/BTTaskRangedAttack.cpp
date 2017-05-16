// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "BTTaskRangedAttack.h"
#include "AIController.h"
#include "Projectiles/PhysicsProjectile.h"

#include "BehaviorTree/BTFunctionLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MlgGameplayStatics.h"


UBTTaskRangedAttack::UBTTaskRangedAttack()
	: ProjectileClass(APhysicsProjectile::StaticClass())
	, SocketName("gunSocket")
{
	NodeName = "BTTaskRangedAttack";
	bCreateNodeInstance = true;
	ConstructorHelpers::FObjectFinder<USoundCue> rangedAttackSoundCueFinder(TEXT("SoundCue'/Game/MVRCFPS_Assets/Sounds/Ranged_Enemy_Attack_Cue.Ranged_Enemy_Attack_Cue'"));
	RangedAttackSoundCue = rangedAttackSoundCueFinder.Object;
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
	USkeletalMeshComponent* meshComponent = pawn->FindComponentByClass<USkeletalMeshComponent>();

	const FTransform socketTransform = meshComponent->GetSocketTransform(SocketName);	

	FVector shotLocation = socketTransform.GetLocation();

	FVector targetDir = targetActor->GetActorLocation() - shotLocation;
	targetDir.Normalize();

	ProjectileClass.GetDefaultObject()->FireProjectile(shotLocation, targetDir, pawn, controller);
	UMlgGameplayStatics::PlaySoundAtLocationNetworked(pawn->GetWorld(), FSoundParams(RangedAttackSoundCue, shotLocation));

	return EBTNodeResult::Succeeded;
}
