// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/WidgetComponent.h"
#include "CooldownWidgetComponent.generated.h"

UENUM(BlueprintType)
namespace ECooldownWidgetShape // Determines how much angle for hit test can deviate
{
	enum Type
	{
		Full			UMETA(DisplayName = "Full"),	// When one ability is used on the hand (Full circle).
		Half			UMETA(DisplayName = "Half"),	// When two abilities are used (half circle).
		Quarter			UMETA(DisplayName = "Quarter")  // When four abilities are used (quarter circle).
	};
}

UENUM(BlueprintType)
namespace ECooldownWidgetLocation // Determines from were angle for hit test is measured
{
	enum Type
	{
		Center			UMETA(DisplayName = "Center"),	// Full Circle
		Top				UMETA(DisplayName = "Top"),		// Half & Quarter Circle
		Bottom			UMETA(DisplayName = "Bottom"),  // Half & Quarter Circle
		Left			UMETA(DisplayName = "Left"),	// Half & Quarter circle
		Right			UMETA(DisplayName = "Right"),   // Half & Quarter circle
	};
}

class UGameplayAbility;

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class MAJORLEAGUEGLADIATOR_API UCooldownWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	UCooldownWidgetComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void SetTouchInputX(float ValueX);
	void SetTouchInputY(float ValueY);

	void SetActivated(TSubclassOf<UGameplayAbility> AbilityType, bool Activated);
	void SetUsed(TSubclassOf<UGameplayAbility> AbilityType, float CooldownSeconds);

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayAbility> RelatedAbilityType;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECooldownWidgetShape::Type> WidgetShape;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECooldownWidgetLocation::Type> WidgetLocation;
	
private:
	UPROPERTY(EditAnywhere)
	UTexture* AbilityIcon;

	void SelectWidget();
	void UnselectWidget();
	void ActivateWidget();
	void DeactivateWidget();
	FVector GetVectorFromWidgetLocation();

	FVector touchInputVector;
	UMaterialInterface* materialInterface;
};
