// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayAbility.h"
#include "Engine/DataAsset.h"
#include "MlgAbilitySet.generated.h"

UENUM(BlueprintType)
namespace EMlgAbilityInputBinds
{
	enum Type
	{
		Ability1				UMETA(DisplayName = "Ability1 (Left Touchpad Top)"),
		Ability2				UMETA(DisplayName = "Ability2 (Left Touchpad Bottom)"),
		Ability3				UMETA(DisplayName = "Ability3 (Left Trigger)"),
		Ability4				UMETA(DisplayName = "Ability4 (Left Grip Button)"),
		Ability5				UMETA(DisplayName = "Ability5 (Right Touch Pad Press)"),
		Ability6				UMETA(DisplayName = "Ability6 (Right Grip Button)"),
		Ability7				UMETA(DisplayName = "Ability7"),
		Ability8				UMETA(DisplayName = "Ability8"),
		Ability9				UMETA(DisplayName = "Ability9"),
		ConfirmAbility			UMETA(DisplayName = "ConfirmAbility"),
		CancelAbility			UMETA(DisplayName = "CancelAbility")
	};
}

/**
*	Example struct that pairs a enum input command to a GameplayAbilityClass.6
*/
USTRUCT()
struct FAbilityBindInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = BindInfo)
	TEnumAsByte<EMlgAbilityInputBinds::Type> Command;

	UPROPERTY(EditAnywhere, Category = BindInfo)
	TSubclassOf<UGameplayAbility> GameplayAbilityClass;
};

// Define here what ability input corresponds to which ability class
// This is pretty similar to GameplayAbilitySet, but altered for our purposes
UCLASS(Blueprintable)
class MAJORLEAGUEGLADIATOR_API UMlgAbilitySet : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UMlgAbilitySet();

	UPROPERTY(EditAnywhere, Category = AbilitySet)
	TArray<FAbilityBindInfo> Abilities;

	void GiveAbilities(UAbilitySystemComponent* AbilitySystemComponent) const;

	void BindAbilitiesToInput(UInputComponent* PlayerInputComponent, UAbilitySystemComponent* AbilitySystemComponent) const;
	
};
