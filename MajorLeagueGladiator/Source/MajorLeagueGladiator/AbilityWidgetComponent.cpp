// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "AbilityWidgetComponent.h"
#include "GameplayAbility.h"

UAbilityWidgetComponent::UAbilityWidgetComponent()
	: touchInputVector(FVector::ZeroVector)
	, totalCooldown(0.0f)
	, remainingCooldown(0.0f)
	, bIsActivated(false)
{
	PrimaryComponentTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UMaterialInterface> material(TEXT("Material'/Game/Materials/M_AbilityUI_World.M_AbilityUI_World'"));
	materialInterface = material.Object;
}

void UAbilityWidgetComponent::BeginPlay()
{
	Super::BeginPlay();
	SetMaterial(0, materialInterface);
	materialInstance = CreateAndSetMaterialInstanceDynamic(0);
}

FVector UAbilityWidgetComponent::GetVectorFromWidgetLocation()
{
	FVector widgetLocation;

	switch (WidgetLocation)
	{
	case ECooldownWidgetLocation::Top:
	{
		widgetLocation = FVector(0.0f, 1.0f, 0.0f);
		break;
	}

	case ECooldownWidgetLocation::Bottom:
	{
		widgetLocation = FVector(0.0f, -1.0f, 0.0f);
		break;
	}

	case ECooldownWidgetLocation::Left:
	{
		widgetLocation = FVector(1.0f, 0.0f, 0.0f);
	}

	case ECooldownWidgetLocation::Right:
	{
		widgetLocation = FVector(-1.0f, 0.0f, 0.0f);
	}

	default:
	{
		checkNoEntry();
	}
	}
	return widgetLocation;
}

void UAbilityWidgetComponent::SetTouchInputX(float ValueX)
{
	touchInputVector.X = ValueX;
}

void UAbilityWidgetComponent::SetTouchInputY(float ValueY)
{
	touchInputVector.Y = ValueY;
}

void UAbilityWidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (remainingCooldown > 0.0f)
	{
		remainingCooldown -= DeltaTime;
		remainingCooldown = FMath::Max(remainingCooldown, 0.0f);

		materialInstance->SetScalarParameterValue(FName(TEXT("GlowAboveUV")), remainingCooldown / totalCooldown);
	}

	if (bIsActivated || remainingCooldown > 0.0f)
	{
		return;
	}

	if (touchInputVector == FVector::ZeroVector)
	{
		UnselectWidget();
	}
	else
	{
		bool bTouchedThis = false;

		if (WidgetShape == ECooldownWidgetShape::Full)
		{
			bTouchedThis = true;
		}
		else
		{
			const FVector locationVector = GetVectorFromWidgetLocation();
			const float touchAngle = FMath::RadiansToDegrees(FMath::Acos(locationVector.CosineAngle2D(touchInputVector)));

			if (WidgetShape == ECooldownWidgetShape::Half)
			{
				bTouchedThis = FMath::Abs(touchAngle) < 22.5f;
			}
			else // ECooldownWidgetShape::Quarter
			{
				bTouchedThis = FMath::Abs(touchAngle) < 45.0f;
			}
		}

		if (bTouchedThis)
		{
			SelectWidget();
		}
		else
		{
			UnselectWidget();
		}
	}
}

void UAbilityWidgetComponent::OnAbilityActivated(TSubclassOf<UGameplayAbility> AbilityType)
{
	if (AbilityType == RelatedAbilityType)
	{
		ActivateWidget();
	}
}

void UAbilityWidgetComponent::OnAbilityUseFail(TSubclassOf<UGameplayAbility> AbilityType)
{
	if (AbilityType == RelatedAbilityType)
	{
		DeactivateWidget();
	}
}

void UAbilityWidgetComponent::OnAbilityUseSuccess(TSubclassOf<UGameplayAbility> AbilityType, float CooldownSeconds)
{
	if (AbilityType == RelatedAbilityType)
	{
		DeactivateWidget();
		SetUsed(CooldownSeconds);
	}
}

void UAbilityWidgetComponent::SelectWidget()
{
	RelativeLocation.X = -1.0f;
	materialInstance->SetScalarParameterValue(FName(TEXT("GlowStrength")), 25.0f);
}

void UAbilityWidgetComponent::UnselectWidget()
{
	RelativeLocation.X = 0.0f;
	materialInstance->SetScalarParameterValue(FName(TEXT("GlowStrength")), 5.0f);
}

void UAbilityWidgetComponent::ActivateWidget()
{
	bIsActivated = true;

	RelativeLocation.X = -2.5f;
	materialInstance->SetScalarParameterValue(FName(TEXT("GlowStrength")), 50.0f);
}

void UAbilityWidgetComponent::DeactivateWidget()
{
	bIsActivated = false;

	RelativeLocation.X = 0.0f;
	materialInstance->SetScalarParameterValue(FName(TEXT("GlowStrength")), 5.0f);
}

void UAbilityWidgetComponent::SetUsed(float CooldownSeconds)
{
	totalCooldown = CooldownSeconds;
	remainingCooldown = CooldownSeconds;
}





