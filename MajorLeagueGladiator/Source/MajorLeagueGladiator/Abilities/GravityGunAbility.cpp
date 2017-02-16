#include "MajorLeagueGladiator.h"
#include "GravityGunAbility.h"

#include "MlgPlayerCharacter.h"
#include "VRControllerComponent.h"
#include "AbilityTask_SearchActor.h"
#include "AbilityTask_PullTarget.h"
#include "AbilityTask_WaitTargetData.h"
#include "AbilityTask_PullTargetActor.h"


namespace
{
	const char* AIM_SOCKET_NAME = "Aim";
}

UGravityGunAbility::UGravityGunAbility(const FObjectInitializer& ObjectInitializer)
	: PullRange(1000)
	, PullSpeed(500)
	, GrabRange(10)
	, LaunchVelocity(1000)
	, pullTask(nullptr)
	, searchTask(nullptr)
	, gripController(nullptr)
	, gripControllerMesh(nullptr)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	bReplicateInputDirectly = true;
}

void UGravityGunAbility::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (pullTask)
	{
		pullTask->ExternalCancel();
		pullTask = nullptr;
	}
}

void UGravityGunAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (searchTask)
	{
		searchTask->ExternalCancel();
		searchTask = nullptr;
	}
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
	searchTask = UAbilityTask_WaitTargetData::WaitTargetData(this, "Search Task", EGameplayTargetingConfirmation::Custom, AAbilityTask_SearchActor::StaticClass());
	searchTask->ValidData.AddDynamic(this, &UGravityGunAbility::OnSearchSuccessful);
	searchTask->Cancelled.AddDynamic(this, &UGravityGunAbility::OnSearchCancelled);

	AGameplayAbilityTargetActor* spawnedActor;
	if (!searchTask->BeginSpawningActor(this, AAbilityTask_SearchActor::StaticClass(), spawnedActor))
	{
		return;
	}	

	AAbilityTask_SearchActor* searchActor = CastChecked<AAbilityTask_SearchActor>(spawnedActor);

	searchActor->StartLocation.LocationType = EGameplayAbilityTargetingLocationType::SocketTransform;
	searchActor->StartLocation.SourceComponent = gripControllerMesh;
	searchActor->StartLocation.SourceSocketName = AIM_SOCKET_NAME;

	searchActor->MaxRange = PullRange;
	searchActor->IgnoredActors.Add(GetOwningActorFromActorInfo());	

	searchTask->FinishSpawningActor(this, spawnedActor);
}

void UGravityGunAbility::OnSearchSuccessful(const FGameplayAbilityTargetDataHandle& Data)
{
	searchTask = nullptr;
	if(GetOwningActorFromActorInfo()->Role < ROLE_Authority)
	{
		return;
	}

	AActor* foundActor = Data.Data[0]->GetActors()[0].Get();

	//This gets spawned on server and client, if he started the ability so that he has a prediction	
	pullTask = UAbilityTask_PullTarget::Create(this, "Pull Actor Task", foundActor, gripController, PullSpeed, GrabRange);
	pullTask->Activate();
	pullTask->OnSuccess.AddUObject(this, &UGravityGunAbility::OnActorPullFinished);
	pullTask->OnFail.AddUObject(this, &UGravityGunAbility::OnActorPullFailed);
}


void UGravityGunAbility::OnSearchCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	searchTask = nullptr;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UGravityGunAbility::OnActorPullFinished(AActor* pulledActor)
{
	pullTask = nullptr;
	if (GetOwningActorFromActorInfo()->Role >= ROLE_Authority)
	{
		gripController->TryGrabActor(pulledActor);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UGravityGunAbility::OnActorPullFailed()
{
	pullTask = nullptr;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UGravityGunAbility::LaunchGrippedActor()
{
	if (GetOwningActorFromActorInfo()->Role >= ROLE_Authority)
	{
		FVector velocity = gripControllerMesh->GetSocketRotation(AIM_SOCKET_NAME).Vector() * LaunchVelocity;
		gripController->LaunchActor(velocity, true);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UGravityGunAbility::SetGripControllerFromOwner()
{
	AActor* owner = GetOwningActorFromActorInfo();
	AMlgPlayerCharacter* mlgPlayerCharacter = CastChecked<AMlgPlayerCharacter>(owner);

	gripController = mlgPlayerCharacter->GetMotionController(EControllerHand::Left);
	gripControllerMesh = mlgPlayerCharacter->GetMotionControllerMesh(EControllerHand::Left);
}

bool UGravityGunAbility::HasGrippedActor() const
{
	check(gripController);
	return gripController->GrippedActors.Num() > 0;
}
