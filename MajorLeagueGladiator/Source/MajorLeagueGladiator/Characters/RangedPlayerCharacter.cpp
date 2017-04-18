// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "RangedPlayerCharacter.h"
#include "AbilityWidgetComponent.h"
#include "GameplayAbility.h"
#include "Abilities/JumpAbility.h"
#include "Abilities/DashAbility.h"

namespace 
{
	const char* NO_COLLISION_PROFILE_NAME = "NoCollision";
}

ARangedPlayerCharacter::ARangedPlayerCharacter(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	ConstructorHelpers::FObjectFinder<UMaterialInterface> multiMat(TEXT("Material'/Game/Materials/M_RangedMultiTool.M_RangedMultiTool'"));
	multiToolMaterial = multiMat.Object;

	ConstructorHelpers::FObjectFinder<UMaterialInterface> bodyMat(TEXT("Material'/Game/Materials/M_DPS.M_DPS'"));
	bodyMaterial = bodyMat.Object;

	auto createWidget = [&](TSubclassOf<UGameplayAbility> boundAbilityType, EAbilityWidgetAngle::Type angle, EAbilityWidgetTriggerLocation::Type triggerLocation,
		const TCHAR* WidgetName, const FName& SocketName) -> UAbilityWidgetComponent*
	{
		UAbilityWidgetComponent* widget = ObjectInitializer.CreateDefaultSubobject<UAbilityWidgetComponent>(this, WidgetName);
		widget->SetupAttachment(leftMesh, SocketName);
		widget->SetCollisionProfileName(NO_COLLISION_PROFILE_NAME);
		widget->WidgetShape = angle;
		widget->WidgetTriggerLocation = triggerLocation;
		widget->RelatedAbilityType = boundAbilityType;
		return widget;
	};

	topAbilityWidget = createWidget(UDashAbility::StaticClass(), EAbilityWidgetAngle::Half, EAbilityWidgetTriggerLocation::Top, TEXT("TopAbilityWidget"), FName(TEXT("Ability_Widget_Top")));
	bottomAbilityWidget = createWidget(UJumpAbility::StaticClass(), EAbilityWidgetAngle::Half, EAbilityWidgetTriggerLocation::Bottom, TEXT("BottomAbilityWidget"), FName(TEXT("Ability_Widget_Bottom")));
}

void ARangedPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	auto instance = UMaterialInstanceDynamic::Create(multiToolMaterial, nullptr);
	auto bodyInstance = UMaterialInstanceDynamic::Create(bodyMaterial, nullptr);
	leftMesh->SetMaterial(0, instance);
	rightMesh->SetMaterial(0, instance);
	headMesh->SetMaterial(0, bodyInstance);
	bodyMesh2->SetMaterial(0, bodyInstance);

	auto setupBindings = [&](UAbilityWidgetComponent* widget)
	{
		OnAbilityUseSuccess.AddDynamic(widget, &UAbilityWidgetComponent::OnAbilityUseSuccess);
		OnAbilityActivated.AddDynamic(widget, &UAbilityWidgetComponent::OnAbilityActivated);
		OnAbilityUseFail.AddDynamic(widget, &UAbilityWidgetComponent::OnAbilityUseFail);
	};

	setupBindings(topAbilityWidget);
	setupBindings(bottomAbilityWidget);
}

void ARangedPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("LeftTouchpadX", this, &ARangedPlayerCharacter::OnLeftTouchpadX);
	PlayerInputComponent->BindAxis("LeftTouchpadY", this, &ARangedPlayerCharacter::OnLeftTouchpadY);
}

void ARangedPlayerCharacter::OnLeftTouchpadX(float Value)
{
	topAbilityWidget->SetTouchInputX(Value);
	bottomAbilityWidget->SetTouchInputX(Value);
}

void ARangedPlayerCharacter::OnLeftTouchpadY(float Value)
{
	topAbilityWidget->SetTouchInputY(Value);
	bottomAbilityWidget->SetTouchInputY(Value);
}



