// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "RangedPlayerCharacter.h"
#include "AbilityWidgetComponent.h"
#include "GameplayAbility.h"
#include "Abilities/JumpAbility.h"
#include "Abilities/DashAbility.h"
#include "Abilities/GrenadeAbility.h"
#include "PlayerDeathComponent.h"
#include "MlgGrippableMeshActor.h"
#include "TriggerZoneComponent.h"

namespace 
{
	const char* NO_COLLISION_PROFILE_NAME = "NoCollision";
	const int32 RANGED_STENCIL_INDEX = 2;
}

ARangedPlayerCharacter::ARangedPlayerCharacter(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	ConstructorHelpers::FObjectFinder<UMaterialInterface> multiMat(TEXT("Material'/Game/Materials/M_RangedMultiTool.M_RangedMultiTool'"));
	multiToolMaterial = multiMat.Object;

	ConstructorHelpers::FObjectFinder<UMaterialInterface> bodyMat(TEXT("Material'/Game/Materials/M_DPS.M_DPS'"));
	bodyMaterial = bodyMat.Object;

	headMesh->CustomDepthStencilValue = RANGED_STENCIL_INDEX;
	bodyMesh2->CustomDepthStencilValue = RANGED_STENCIL_INDEX;

	auto createWidget = [&](USceneComponent* attachTo, TSubclassOf<UGameplayAbility> boundAbilityType, EAbilityWidgetAngle::Type angle, EAbilityWidgetTriggerLocation::Type triggerLocation,
		const TCHAR* WidgetName, const FName& SocketName) -> UAbilityWidgetComponent*
	{
		UAbilityWidgetComponent* widget = ObjectInitializer.CreateDefaultSubobject<UAbilityWidgetComponent>(this, WidgetName);
		widget->SetupAttachment(attachTo, SocketName);
		widget->SetCollisionProfileName(NO_COLLISION_PROFILE_NAME);
		widget->WidgetShape = angle;
		widget->WidgetTriggerLocation = triggerLocation;
		widget->RelatedAbilityType = boundAbilityType;
		return widget;
	};

	lefthandTopAbilityWidget = createWidget(leftMesh, UDashAbility::StaticClass(), EAbilityWidgetAngle::Half, EAbilityWidgetTriggerLocation::Top, TEXT("TopAbilityWidget"), FName(TEXT("Ability_Widget_Top")));
	lefthandBottomAbilityWidget = createWidget(leftMesh, UJumpAbility::StaticClass(), EAbilityWidgetAngle::Half, EAbilityWidgetTriggerLocation::Bottom, TEXT("BottomAbilityWidget"), FName(TEXT("Ability_Widget_Bottom")));
	rightHandAbilityWidget = createWidget(rightMesh, UGrenadeAbility::StaticClass(), EAbilityWidgetAngle::Full, EAbilityWidgetTriggerLocation::Center, TEXT("RighthandAbilityWidget"), FName(TEXT("Ability_Widget_Center")));

	deathComponent->AfterDeathTeleportPosition = FVector(3426.588379, 131.509048, -1396.709106);
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

	setupBindings(lefthandTopAbilityWidget);
	setupBindings(lefthandBottomAbilityWidget);
	setupBindings(rightHandAbilityWidget);
}

void ARangedPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("LeftTouchpadX", this, &ARangedPlayerCharacter::OnLeftTouchpadX);
	PlayerInputComponent->BindAxis("LeftTouchpadY", this, &ARangedPlayerCharacter::OnLeftTouchpadY);
	PlayerInputComponent->BindAxis("RightTouchpadX", this, &ARangedPlayerCharacter::OnRightTouchpadX);
	PlayerInputComponent->BindAxis("RightTouchpadY", this, &ARangedPlayerCharacter::OnRightTouchpadY);
}

void ARangedPlayerCharacter::OnLeftTouchpadX(float Value)
{
	lefthandTopAbilityWidget->SetTouchInputX(Value);
	lefthandBottomAbilityWidget->SetTouchInputX(Value);
}

void ARangedPlayerCharacter::OnLeftTouchpadY(float Value)
{
	lefthandTopAbilityWidget->SetTouchInputY(Value);
	lefthandBottomAbilityWidget->SetTouchInputY(Value);
}

void ARangedPlayerCharacter::OnRightTouchpadX(float Value)
{
	rightHandAbilityWidget->SetTouchInputX(Value);
}

void ARangedPlayerCharacter::OnRightTouchpadY(float Value)
{
	rightHandAbilityWidget->SetTouchInputY(Value);
}

void ARangedPlayerCharacter::ToggleMenuState(bool bMenuEnabled)
{
	Super::ToggleMenuState(bMenuEnabled);

	lefthandTopAbilityWidget->SetVisibility(!bMenuEnabled);
	lefthandBottomAbilityWidget->SetVisibility(!bMenuEnabled);
	rightHandAbilityWidget->SetVisibility(!bMenuEnabled);
	
	auto triggerZone = attachedWeapon->FindComponentByClass<UTriggerZoneComponent>();
	triggerZone->SetVisibility(false);
}
