// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "ShieldAbility.h"
#include "Characters/MlgPlayerCharacter.h"
#include "GameplayAbilityTargetActor_Box.h"
#include "AbilityTask_WaitTargetData.h"
#include "VRControllerComponent.h"
#include "ShieldActor.h"

namespace
{
	const FName AIM_SOCKET_NAME("Aim");
	const FName SHIELD_SOCKET_NAME("Shield");
}

UShieldAbility::UShieldAbility()
	: shieldActorClass(AShieldActor::StaticClass())
	, pushBoxHalfExtent(100, 100, 100)
	, pushBoxOffsetFromController(100, 0, 0)
	, pushVelocity(1000)
{
	bReplicateInputDirectly = true;
}

void UShieldAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UShieldAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	SetGripControllerFromOwner();
	PushAwayCloseActors();
}

void UShieldAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	DespawnShield();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


void UShieldAbility::PushAwayCloseActors()
{
	UAbilityTask_WaitTargetData* searchTask = UAbilityTask_WaitTargetData::WaitTargetData(this, "Search Task", EGameplayTargetingConfirmation::Instant, AGameplayAbilityTargetActor_Box::StaticClass());
	searchTask->ValidData.AddDynamic(this, &UShieldAbility::OnBoxTraceFinished);

	AGameplayAbilityTargetActor* spawnedActor;
	if (!searchTask->BeginSpawningActor(this, AGameplayAbilityTargetActor_Box::StaticClass(), spawnedActor))
	{
		return;
	}

	AGameplayAbilityTargetActor_Box* searchActor = CastChecked<AGameplayAbilityTargetActor_Box>(spawnedActor);

	searchActor->StartLocation.LocationType = EGameplayAbilityTargetingLocationType::SocketTransform;
	searchActor->StartLocation.SourceComponent = gripControllerMesh;
	searchActor->StartLocation.SourceSocketName = AIM_SOCKET_NAME;

	searchActor->SetHalfExtent(pushBoxHalfExtent);
	searchActor->LocationOffsetFromPositon = pushBoxOffsetFromController;
	searchActor->IgnoredActors.Add(GetOwningActorFromActorInfo());

	searchTask->FinishSpawningActor(this, spawnedActor);
}

void UShieldAbility::OnBoxTraceFinished(const FGameplayAbilityTargetDataHandle& Data)
{
	FGameplayAbilityTargetData* targetData = Data.Data[0].Get();
	FGameplayAbilityTargetData_ActorArray* actorArray = static_cast<FGameplayAbilityTargetData_ActorArray*>(targetData);
	const FVector launchVelocity = actorArray->SourceLocation.GetTargetingTransform().GetRotation().Vector() * pushVelocity;
	for (auto actor : actorArray->GetActors())
	{
		ACharacter* character = Cast<ACharacter>(actor.Get());
		if (character)
		{
			character->LaunchCharacter(launchVelocity, true, true);
		}
		else
		{
			UPrimitiveComponent* rootcomp = CastChecked<UPrimitiveComponent>(actor->GetRootComponent());
			rootcomp->AddImpulse(launchVelocity, NAME_None, true);
		}
		
	}
	SpawnShield();
}

void UShieldAbility::SpawnShield()
{
	if (shieldActorClass == nullptr)
	{
		UE_LOG(DebugLog, Warning, TEXT("Shield Class not set"));
	}

	if (GetOwningActorFromActorInfo()->Role >= ROLE_Authority)
	{
		
		APawn* ownerPawn = CastChecked<APawn>(GetOwningActorFromActorInfo());

		FActorSpawnParameters spawnParams;
		spawnParams.Owner = ownerPawn;
		spawnParams.Instigator = ownerPawn;

		if (!gripControllerMesh->DoesSocketExist(SHIELD_SOCKET_NAME))
		{
			UE_LOG(DebugLog, Warning, TEXT("Shield Socket Missing in MotionController Mesh"));
		}
		FTransform shieldSpawnTransform = gripControllerMesh->GetSocketTransform(SHIELD_SOCKET_NAME);

		shieldActor = GetWorld()->SpawnActor<AShieldActor>(shieldActorClass, shieldSpawnTransform, spawnParams);

		gripController->GripActor(shieldActor, shieldSpawnTransform, false);
	}
}

void UShieldAbility::DespawnShield()
{
	if (GetOwningActorFromActorInfo()->Role >= ROLE_Authority && shieldActor)
	{		
		gripController->DropActor(shieldActor, false);
		shieldActor->Destroy();
		shieldActor = nullptr;
	}
}

void UShieldAbility::SetGripControllerFromOwner()
{
	AActor* owner = GetOwningActorFromActorInfo();
	AMlgPlayerCharacter* mlgPlayerCharacter = CastChecked<AMlgPlayerCharacter>(owner);

	gripControllerMesh = mlgPlayerCharacter->GetMotionControllerMesh(EControllerHand::Left);
	gripController = mlgPlayerCharacter->GetMotionController(EControllerHand::Left);
}

