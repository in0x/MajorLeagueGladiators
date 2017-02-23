#include "MajorLeagueGladiator.h"
#include "JumpAbility.h"
#include "AbilityTask_WaitTargetData.h"
#include "GameplayAbilityTargetActor_PredictProjectile.h"

UJumpAbility::UJumpAbility()
	: PredictProjectileSpeed(1000.f)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	bReplicateInputDirectly = true;
}

void UJumpAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (waitForTargetTask)
	{
		// Player has released targeting button -> finished picking target
		targetingSpawnedActor->bShouldBroadcastResult = true;
	}
}

void UJumpAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	waitForTargetTask = UAbilityTask_WaitTargetData::WaitTargetData(this, "Pick Target Task", EGameplayTargetingConfirmation::Custom, AGameplayAbilityTargetActor_PredictProjectile::StaticClass());
	waitForTargetTask->ValidData.AddDynamic(this, &UJumpAbility::OnTargetPickSuccessful);
	waitForTargetTask->Cancelled.AddDynamic(this, &UJumpAbility::OnTargetPickCanceled);

	AGameplayAbilityTargetActor* spawnedActor;
	if (!waitForTargetTask->BeginSpawningActor(this, AGameplayAbilityTargetActor_PredictProjectile::StaticClass(), spawnedActor))
	{
		return;
	}

	targetingSpawnedActor = CastChecked<AGameplayAbilityTargetActor_PredictProjectile>(spawnedActor);

	targetingSpawnedActor->TargetProjectileSpeed = PredictProjectileSpeed;
	targetingSpawnedActor->IgnoredActors.Add(GetOwningActorFromActorInfo());
	targetingSpawnedActor->targetingType = EPickMoveLocationTargeting::FromPlayerCapsule;

	waitForTargetTask->FinishSpawningActor(this, spawnedActor);
}

void UJumpAbility::OnTargetPickSuccessful(const FGameplayAbilityTargetDataHandle& Data)
{
	auto player = CastChecked<ACharacter>(GetOwningActorFromActorInfo());
	player->LaunchCharacter(Data.Data[0]->GetHitResult()->TraceStart, true, true); // Reuse TraceStart to transport launch velocity

	waitForTargetTask = nullptr;
	targetingSpawnedActor = nullptr;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UJumpAbility::OnTargetPickCanceled(const FGameplayAbilityTargetDataHandle& Data)
{
	waitForTargetTask = nullptr;
	targetingSpawnedActor = nullptr;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

