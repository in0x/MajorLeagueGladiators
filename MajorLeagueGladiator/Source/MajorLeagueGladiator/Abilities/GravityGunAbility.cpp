#include "MajorLeagueGladiator.h"
#include "GravityGunAbility.h"

#include "Characters/MlgPlayerCharacter.h"
#include "VRControllerComponent.h"
#include "GameplayAbilityTargetActor_Cone.h"
#include "AbilityTask_PullTarget.h"
#include "AbilityTask_WaitTargetData.h"
#include "CollisionStatics.h"

namespace
{
	const FName AIM_SOCKET_NAME("Aim");
}

UGravityGunAbility::UGravityGunAbility(const FObjectInitializer& ObjectInitializer)
	: PullRange(1000)
	, PullSpeed(500)
	, GrabRange(10)
	, LaunchVelocity(1000)
	, HalfAngleDegrees(10.f)
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
	searchTask = UAbilityTask_WaitTargetData::WaitTargetData(this, "Search Task", EGameplayTargetingConfirmation::Custom, AGameplayAbilityTargetActor_Cone::StaticClass());
	searchTask->ValidData.AddDynamic(this, &UGravityGunAbility::OnSearchSuccessful);
	searchTask->Cancelled.AddDynamic(this, &UGravityGunAbility::OnSearchCancelled);

	AGameplayAbilityTargetActor* spawnedActor;
	if (!searchTask->BeginSpawningActor(this, AGameplayAbilityTargetActor_Cone::StaticClass(), spawnedActor))
	{
		return;
	}	

	auto* searchActor = CastChecked<AGameplayAbilityTargetActor_Cone>(spawnedActor);

	searchActor->StartLocation.LocationType = EGameplayAbilityTargetingLocationType::SocketTransform;
	searchActor->StartLocation.SourceComponent = gripControllerMesh;
	searchActor->StartLocation.SourceSocketName = AIM_SOCKET_NAME;

	searchActor->Distance = PullRange;
	searchActor->AutoConfirm = true;
	searchActor->IsVisualizingCone = true;
	searchActor->SetHalfAngleDegrees(HalfAngleDegrees);

	searchActor->IgnoredActors.Add(GetOwningActorFromActorInfo());

	searchActor->CollisionChannel = CollisionStatics::GetCollsionChannelByName(CollisionStatics::GRIPSCAN_TRACE_CHANNEL_NAME);

	searchTask->FinishSpawningActor(this, spawnedActor);
}

void UGravityGunAbility::OnSearchSuccessful(const FGameplayAbilityTargetDataHandle& Data)
{
	searchTask = nullptr;

	AActor* foundActor = Data.Data[0]->GetActors()[0].Get();

	//This gets spawned on server and client, if he started the ability so that he has a prediction	
	pullTask = UAbilityTask_PullTarget::Create(this, "Pull Actor Task", foundActor, gripController, PullSpeed, GrabRange);
	pullTask->OnSuccess.AddUObject(this, &UGravityGunAbility::OnActorPullFinished);
	pullTask->OnFail.AddUObject(this, &UGravityGunAbility::OnActorPullFailed);
	pullTask->ReadyForActivation();
}

void UGravityGunAbility::OnSearchCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	searchTask = nullptr;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGravityGunAbility::OnActorPullFinished(AActor* pulledActor)
{
	pullTask = nullptr;
	if (GetOwningActorFromActorInfo()->Role >= ROLE_Authority)
	{
		gripController->TryGrabActor(pulledActor);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGravityGunAbility::OnActorPullFailed()
{
	pullTask = nullptr;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGravityGunAbility::LaunchGrippedActor()
{
	if (GetOwningActorFromActorInfo()->Role >= ROLE_Authority)
	{
		FVector velocity = gripControllerMesh->GetSocketRotation(AIM_SOCKET_NAME).Vector() * LaunchVelocity;
		gripController->LaunchActor(velocity, true);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
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
