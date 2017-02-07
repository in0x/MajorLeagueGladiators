// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "TargetingGameplayAbility.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"

#include "AbilitySystemExample/GameplayAbilityTargetActor_MotionControl.h"


void UTargetingGameplayAbility::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("Ability Input Pressed"));
}
void UTargetingGameplayAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("Ability Input Released"));
}

void UTargetingGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("Ability Activated"));
	auto* task = UAbilityTask_WaitTargetData::WaitTargetData(this, "Task", EGameplayTargetingConfirmation::UserConfirmed, AGameplayAbilityTargetActor_MotionControl::StaticClass());

	task->ValidData.AddDynamic(this, &UTargetingGameplayAbility::OnDataReceived);

	AGameplayAbilityTargetActor* SpawnedActor = nullptr;
	task->BeginSpawningActor(this, AGameplayAbilityTargetActor_MotionControl::StaticClass(), SpawnedActor);
	task->FinishSpawningActor(this, SpawnedActor);
}

void UTargetingGameplayAbility::OnDataReceived(const FGameplayAbilityTargetDataHandle& Data)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("target Data received"));

	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), CurrentActivationInfo, true);	
}