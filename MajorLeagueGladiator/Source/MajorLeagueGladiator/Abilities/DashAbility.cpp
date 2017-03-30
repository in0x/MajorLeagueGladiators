#include "MajorLeagueGladiator.h"
#include "DashAbility.h"
#include "Characters/MlgPlayerCharacter.h"
#include "VRControllerComponent.h"
#include "AbilityTask_WaitTargetData.h"
#include "GameplayAbilityTargetActor_Raycast.h"
#include "AbilityTask_MoveTo.h"

namespace
{
	const char* AIM_SOCKET_NAME = "Aim";
}

UDashAbility::UDashAbility()
	: MaxRange(1000.f)
	, MoveSpeed(1500.f)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	bReplicateInputDirectly = true;
}

void UDashAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (waitForTargetTask && targetingSpawnedActor)
	{
		// Player has released targeting button -> finished picking target
		waitForTargetTask->ExternalConfirm(true);
	}
}

void UDashAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	waitForTargetTask = UAbilityTask_WaitTargetData::WaitTargetData(this, "Pick Target Task", EGameplayTargetingConfirmation::Custom, AGameplayAbilityTargetActor_Raycast::StaticClass());
	waitForTargetTask->ValidData.AddDynamic(this, &UDashAbility::OnTargetPickSuccessful);
	waitForTargetTask->Cancelled.AddDynamic(this, &UDashAbility::OnTargetPickCanceled);

	AGameplayAbilityTargetActor* spawnedActor;
	if (!waitForTargetTask->BeginSpawningActor(this, AGameplayAbilityTargetActor_Raycast::StaticClass(), spawnedActor))
	{
		return;
	}

	targetingSpawnedActor = CastChecked<AGameplayAbilityTargetActor_Raycast>(spawnedActor);

	auto player = CastChecked<AMlgPlayerCharacter>(GetOwningActorFromActorInfo());	
	auto gripController = player->GetMotionControllerMesh(EControllerHand::Left);
	targetingSpawnedActor->StartLocation.SourceComponent = gripController;
	targetingSpawnedActor->StartLocation.LocationType = EGameplayAbilityTargetingLocationType::SocketTransform;
	targetingSpawnedActor->StartLocation.SourceSocketName = AIM_SOCKET_NAME;

	targetingSpawnedActor->aimDirection = ERaycastTargetDirection::ComponentRotation;
	targetingSpawnedActor->IgnoredActors.Add(GetOwningActorFromActorInfo());
	targetingSpawnedActor->MaxRange = MaxRange;

	targetingSpawnedActor->EvalTargetFunc = [](const FHitResult& result)
	{
		auto isNotVertical = FVector::DotProduct(result.ImpactNormal, FVector(0,0,1)) > 0.7f; 
		auto hitSurface = result.bBlockingHit > 0;
		return isNotVertical && hitSurface;
	};

	waitForTargetTask->FinishSpawningActor(this, spawnedActor);
}

void UDashAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	waitForTargetTask = nullptr;
	targetingSpawnedActor = nullptr;
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UDashAbility::OnTargetPickSuccessful(const FGameplayAbilityTargetDataHandle& Data)
{
	if (GetOwningActorFromActorInfo()->HasAuthority())
	{
		auto player = CastChecked<AMlgPlayerCharacter>(GetOwningActorFromActorInfo());	
		auto capsule = player->GetCapsuleComponent();

		// Add capsule halfheight to Z since our "location" is the capsule 
		// center, otherwise we would move our body into the ground.
		auto location = Data.Data[0]->GetHitResult()->Location;	
		location += capsule->GetUpVector() * capsule->GetScaledCapsuleHalfHeight();
		
		moveToTask = UAbilityTask_MoveTo::Create(this, "MoveTo Task", location, MoveSpeed, player);
		moveToTask->Activate();
		moveToTask->OnLocationReached.AddUObject(this, &UDashAbility::OnLocationReached);

		player->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		player->EnableActorCollison_NetMulticast(false);
		player->SetAbilityMoveTargetLocation(location);
	}
}

void UDashAbility::OnTargetPickCanceled(const FGameplayAbilityTargetDataHandle& Data)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UDashAbility::OnLocationReached()
{
	auto player = CastChecked<AMlgPlayerCharacter>(GetOwningActorFromActorInfo());
	player->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	player->EnableActorCollison_NetMulticast(true);
	player->InvalidateAbilityMoveTargetLocation();

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}


