#include "MajorLeagueGladiator.h"
#include "DashAbility.h"
#include "MlgPlayerCharacter.h"
#include "VRControllerComponent.h"
#include "AbilityTask_WaitTargetData.h"
#include "GameplayAbilityTargetActor_PredictProjectile.h"
#include "GameplayTask_WaitUntiLocationReached.h"

UDashAbility::UDashAbility()
	: PredictProjectileSpeed(1000.f)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	bReplicateInputDirectly = true;
}

void UDashAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (waitForTargetTask)
	{
		// Player has released targeting button -> finished picking target
		targetingSpawnedActor->bShouldBroadcastResult = true;
	}
}

void UDashAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	waitForTargetTask = UAbilityTask_WaitTargetData::WaitTargetData(this, "Pick Target Task", EGameplayTargetingConfirmation::Custom, AGameplayAbilityTargetActor_PredictProjectile::StaticClass());
	waitForTargetTask->ValidData.AddDynamic(this, &UDashAbility::OnTargetPickSuccessful);
	waitForTargetTask->Cancelled.AddDynamic(this, &UDashAbility::OnTargetPickCanceled);

	AGameplayAbilityTargetActor* spawnedActor;
	if (!waitForTargetTask->BeginSpawningActor(this, AGameplayAbilityTargetActor_PredictProjectile::StaticClass(), spawnedActor))
	{
		return;
	}

	targetingSpawnedActor = CastChecked<AGameplayAbilityTargetActor_PredictProjectile>(spawnedActor);

	targetingSpawnedActor->TargetProjectileSpeed = PredictProjectileSpeed;
	targetingSpawnedActor->IgnoredActors.Add(GetOwningActorFromActorInfo());
	targetingSpawnedActor->targetingType = EPickMoveLocationTargeting::FromPlayerCapsule /* TODO(Phil) FromController -> Borked */;

	waitForTargetTask->FinishSpawningActor(this, spawnedActor);
}

void UDashAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	waitForTargetTask = nullptr;
	targetingSpawnedActor = nullptr;
	waitForLocationReachTask = nullptr;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UDashAbility::OnTargetPickSuccessful(const FGameplayAbilityTargetDataHandle& Data)
{
	if (GetOwningActorFromActorInfo()->Role >= ROLE_Authority)
	{
		auto player = CastChecked<AMlgPlayerCharacter>(GetOwningActorFromActorInfo());
		player->ExtendedSimpleMoveToLocation(Data.Data[0]->GetHitResult()->Location);

		/*waitForLocationReachTask = UGameplayTask_WaitUntiLocationReached::Create(this, "Wait for Location Reached Task", player);
		waitForLocationReachTask->Activate();

		waitForLocationReachTask->OnLocationReached.AddLambda([this]() 
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		});*/

		auto compPathFollow = player->GetController()->GetComponentByClass(UPathFollowingComponent::StaticClass());
		auto pathFollow = CastChecked<UPathFollowingComponent>(compPathFollow);
		pathFollow->OnRequestFinished.AddUObject(this, &UDashAbility::OnLocationReached);
	}
}

void UDashAbility::OnLocationReached(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UDashAbility::OnTargetPickCanceled(const FGameplayAbilityTargetDataHandle& Data)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

