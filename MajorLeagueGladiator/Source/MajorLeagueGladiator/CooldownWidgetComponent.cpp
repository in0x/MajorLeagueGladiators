// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "CooldownWidgetComponent.h"

UCooldownWidgetComponent::UCooldownWidgetComponent()
	: touchInputVector(FVector::ZeroVector)
{
	PrimaryComponentTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UMaterialInterface> material(TEXT("Material'/Game/Materials/M_AbilityUI_World.M_AbilityUI_World'"));
	materialInterface = material.Object;
}

FVector UCooldownWidgetComponent::GetVectorFromWidgetLocation()
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

void UCooldownWidgetComponent::SetTouchInputX(float ValueX)
{
	touchInputVector.X = ValueX;
}

void UCooldownWidgetComponent::SetTouchInputY(float ValueY)
{
	touchInputVector.Y = ValueY;
}

void UCooldownWidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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

void UCooldownWidgetComponent::SetActivated(TSubclassOf<UGameplayAbility> AbilityType, bool Activated)
{
	if (Activated)
	{
		ActivateWidget();
	}
	else
	{
		DeactivateWidget();
	}
}

void UCooldownWidgetComponent::SetUsed(TSubclassOf<UGameplayAbility> AbilityType, float CooldownSeconds)
{
	// Start cooldown effect
}

void UCooldownWidgetComponent::SelectWidget()
{
	RelativeLocation.Z = 5.0f;
	UE_LOG(DebugLog, Log, TEXT("%s selected"), *GetName());
}

void UCooldownWidgetComponent::UnselectWidget()
{
	RelativeLocation.Z = 0.0f;
}

void UCooldownWidgetComponent::ActivateWidget()
{
}

void UCooldownWidgetComponent::DeactivateWidget()
{
}
