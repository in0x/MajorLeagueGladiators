// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "ShieldAbility.h"
#include "MlgPlayerCharacter.h"
#include "GameplayAbilityTargetActor_Box.h"
#include "AbilityTask_WaitTargetData.h"
#include "VRControllerComponent.h"
#include "ShieldActor.h"


UShieldAbility::UShieldAbility()
	: shieldActorClass(AShieldActor::StaticClass())
	, pushBoxHalfExtent(100, 100, 100)
	, pushBoxOffsetFromController(100, 0, 0)
	, pushVelocity(1000)
{
}

void UShieldAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UShieldAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	SetGripControllerFromOwner();
	PushAwayCloseActors();
}

void UShieldAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility)
{
	DespawnShield();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility);
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
	searchActor->StartLocation.SourceSocketName = "Aim";

	searchActor->SetHalfExtent(pushBoxHalfExtent);
	searchActor->LocationOffsetFromPositon = pushBoxOffsetFromController;
	searchActor->IgnoredActors.Add(GetOwningActorFromActorInfo());

	searchTask->FinishSpawningActor(this, spawnedActor);
}

void UShieldAbility::OnBoxTraceFinished(const FGameplayAbilityTargetDataHandle& Data)
{
	FGameplayAbilityTargetData* targetData = Data.Data[0].Get();
	FGameplayAbilityTargetData_ActorArray* actorArray = static_cast<FGameplayAbilityTargetData_ActorArray*>(targetData);
	const FVector impulse = actorArray->SourceLocation.GetTargetingTransform().GetRotation().Vector() * pushVelocity;
	for (auto actor : actorArray->GetActors())
	{
		ACharacter* character = Cast<ACharacter>(actor.Get());
		if (character)
		{
			character->LaunchCharacter(impulse, true, true);
		}
		else
		{
			UPrimitiveComponent* rootcomp = CastChecked<UPrimitiveComponent>(actor->GetRootComponent());
			rootcomp->AddImpulse(impulse, NAME_None, true);
		}
		
	}
	SpawnShield();
}

void UShieldAbility::SpawnShield()
{
	if (GetOwningActorFromActorInfo()->Role >= ROLE_Authority && shieldActorClass)
	{
		FActorSpawnParameters spawnParameters;
		shieldActor = GetWorld()->SpawnActor<AShieldActor>(shieldActorClass, GetOwningActorFromActorInfo()->GetTransform());		
		
		FTransform gripTransform = gripControllerMesh->GetSocketTransform("Aim");

		gripController->GripActor(shieldActor,
			gripTransform,
			false,
			"Ability");
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

