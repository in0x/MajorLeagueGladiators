// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgAbilitySet.h"
#include "AbilitySystemComponent.h"
#include "GravityGunAbility.h"

namespace
{
	constexpr int32 LEVEL_1 = 1;
}


UMlgAbilitySet::UMlgAbilitySet()
{
	Abilities.Add(FAbilityBindInfo{ EMlgAbilityInputBinds::Ability1, UGravityGunAbility::StaticClass() });
}

void UMlgAbilitySet::GiveAbilities(UAbilitySystemComponent* AbilitySystemComponent) const
{
	
	for (const FAbilityBindInfo& BindInfo : Abilities)
	{

		checkf (BindInfo.GameplayAbilityClass, TEXT("GameplayAbilityClass %d not set"), static_cast<int32>(BindInfo.Command))			

		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(
			BindInfo.GameplayAbilityClass->GetDefaultObject<UGameplayAbility>(),
			LEVEL_1,
			static_cast<int32>(BindInfo.Command)));		
	}
}

void UMlgAbilitySet::BindAbilitiesToInput(UInputComponent* PlayerInputComponent, UAbilitySystemComponent* AbilitySystemComponent) const
{
	FGameplayAbiliyInputBinds inputBinds("", "",
		"EMlgAbilityInputBinds",
		static_cast<int32>(EMlgAbilityInputBinds::ConfirmAbility),
		static_cast<int32>(EMlgAbilityInputBinds::CancelAbility)
		);

	AbilitySystemComponent->BindAbilityActivationToInputComponent(PlayerInputComponent, inputBinds);

}
