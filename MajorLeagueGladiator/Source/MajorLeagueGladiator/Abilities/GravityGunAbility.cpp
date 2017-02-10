#include "MajorLeagueGladiator.h"
#include "GravityGunAbility.h"

#include "MlgPlayerCharacter.h"
#include "VRControllerComponent.h"
#include "AbilityTask_Search.h"
#include "AbilityTask_SearchActor.h"
#include "AbilityTask_PullTarget.h"
#include "AbilityTask_WaitTargetData.h"
#include "AbilityTask_PullTargetActor.h"


UGravityGunAbility::UGravityGunAbility(const FObjectInitializer& ObjectInitializer)
	: gripController(nullptr)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	bReplicateInputDirectly = true;
}

void UGravityGunAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (currentTask.IsValid() && !currentTask.IsStale())
	{
		currentTask->ExternalCancel();
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, true);
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
	UAbilityTask_WaitTargetData* searchTask = UAbilityTask_WaitTargetData::WaitTargetData(this, "Search Task", EGameplayTargetingConfirmation::Custom, AAbilityTask_SearchActor::StaticClass());
	searchTask->ValidData.AddDynamic(this, &UGravityGunAbility::OnSearchSuccessful);
	currentTask = searchTask;


	AGameplayAbilityTargetActor* spawnedActor;
	if (!searchTask->BeginSpawningActor(this, AAbilityTask_SearchActor::StaticClass(), spawnedActor))
	{
		return;
	}	

	AAbilityTask_SearchActor* searchActor = CastChecked<AAbilityTask_SearchActor>(spawnedActor);

	searchActor->TargetingSceneComponent = gripController;
	searchActor->maxRange = 1000;
	searchActor->IgnoredActors.Add(GetOwningActorFromActorInfo());	

	searchTask->FinishSpawningActor(this, spawnedActor);

}

void UGravityGunAbility::LaunchGrippedActor()
{
	if (GetOwningActorFromActorInfo()->Role >= ROLE_Authority)
	{
		AActor* actorToLaunch = CastChecked<AActor>(gripController->GrippedActors[0].GrippedObject);
		gripController->DropAllGrips();

		UPrimitiveComponent* rootComp = Cast<UPrimitiveComponent>(actorToLaunch->GetRootComponent());
		if (rootComp)
		{
			//should also probably ignore the player char so it doesn't bounce of it's own hands
			bool ignoreMass = true;
			rootComp->AddImpulse(rootComp->GetForwardVector() * 1000, NAME_None, ignoreMass);
		}
		
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UGravityGunAbility::OnSearchSuccessful(const FGameplayAbilityTargetDataHandle& Data)
{
	AActor* foundActor = Data.Data[0]->GetActors()[0].Get();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Start Pulling"));

	//This gets spawned on server and client, if he started the ability so that he has a prediction	
	UAbilityTask_PullTarget* pullTask = UAbilityTask_PullTarget::Create(this, "Pull Actor Task", foundActor, gripController, 100, 10);
	pullTask->Activate();
	pullTask->OnSuccess.AddUObject(this, &UGravityGunAbility::OnActorPullFinished);
	currentTask = pullTask;

}

void UGravityGunAbility::OnActorPullFinished(AActor* pulledActor)
{
	if (GetOwningActorFromActorInfo()->Role >= ROLE_Authority)
	{
		gripController->TryGrabActor(pulledActor);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
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
