#include "MajorLeagueGladiator.h"
#include "GravityGunAbility.h"

#include "MlgPlayerCharacter.h"
#include "VRControllerComponent.h"
#include "AbilityTask_Search.h"
#include "AbilityTask_SearchActor.h"
#include "AbilityTask_PullTarget.h"


UGravityGunAbility::UGravityGunAbility(const FObjectInitializer& ObjectInitializer)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	bReplicateInputDirectly = true;
}

void UGravityGunAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (currentTask.IsValid() && !currentTask.IsStale())
	{
		currentTask->ExternalCancel();
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, false);
}

void UGravityGunAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData * TriggerEventData)
{
	SetGripControllerFromOwner();
	if (!HasGrippedActor())
	{
		SearchAndPull();
	}
	else
	{
		LaunchGrippedActor();
	}
}

void UGravityGunAbility::SearchAndPull()
{
	UAbilityTask_Search* searchTask = UAbilityTask_Search::Create(this, "Search Task");
	AAbilityTask_SearchActor* spawnedActor;
	searchTask->BeginSpawningActor(this, AAbilityTask_SearchActor::StaticClass(), spawnedActor);
	searchTask->OnSuccess.AddUObject(this, &UGravityGunAbility::OnActorFound);

	spawnedActor->TargetingSceneComponent = gripController;
	spawnedActor->maxRange = 1000;
	spawnedActor->IgnoredActors.Add(GetOwningActorFromActorInfo());

	searchTask->FinishSpawningActor(this, spawnedActor);
	currentTask = searchTask;
}

void UGravityGunAbility::LaunchGrippedActor()
{
	if (GetOwningActorFromActorInfo()->Role >= ROLE_Authority)
	{
		AActor* actorToLaunch = gripController->GrippedActors[0].Actor;
		gripController->DropAllGrips();

		UPrimitiveComponent* rootComp = Cast<UPrimitiveComponent>(actorToLaunch->GetRootComponent());
		if (rootComp)
		{
			//should also probably ignore the player char so it doesn't bounce of it's own hands
			bool ignoreMass = true;
			rootComp->AddImpulse(rootComp->GetForwardVector() * 1000, NAME_None, ignoreMass);
		}
		
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false);
}

void UGravityGunAbility::OnActorFound(AActor* Actor)
{
	UAbilityTask_PullTarget* pullTask = UAbilityTask_PullTarget::Create(this, "Pull Actor Task", Actor, gripController, 100, 10);
	pullTask->Activate();
	pullTask->OnSuccess.AddUObject(this, &UGravityGunAbility::OnActorPullFinished);
	currentTask = pullTask;
}

void UGravityGunAbility::OnActorPullFinished(AActor* Actor)
{
	if (GetOwningActorFromActorInfo()->Role >= ROLE_Authority)
	{
		gripController->TryGrabActor(Actor);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false);
}

void UGravityGunAbility::SetGripControllerFromOwner()
{
	AActor* owner = GetOwningActorFromActorInfo();
	AMlgPlayerCharacter* mlgPlayerCharacter = CastChecked<AMlgPlayerCharacter>(owner);

	gripController = CastChecked<UVRControllerComponent>(mlgPlayerCharacter->LeftMotionController);
}

bool UGravityGunAbility::HasGrippedActor() const
{
	check(gripController);
	return gripController->GrippedActors.Num() > 0;
}
