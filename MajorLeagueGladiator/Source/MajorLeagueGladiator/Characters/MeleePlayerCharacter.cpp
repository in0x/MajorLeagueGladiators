// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MeleePlayerCharacter.h"
#include "AbilityWidgetComponent.h"
#include "Abilities/DashAbility.h"
#include "Abilities/JumpDashAbility.h"
#include "Animation/BlendSpace1D.h"
#include "PlayerDeathComponent.h"

namespace
{
	const char* NO_COLLISION_PROFILE_NAME = "NoCollision";
}

AMeleePlayerCharacter::AMeleePlayerCharacter(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	ConstructorHelpers::FObjectFinder<UMaterialInterface> multiMat(TEXT("Material'/Game/Materials/M_MeleeMultiTool.M_MeleeMultiTool'"));
	multiToolMaterial = multiMat.Object;
	ConstructorHelpers::FObjectFinder<UMaterialInterface> swordMat(TEXT("Material'/Game/Materials/M_Sword.M_Sword'"));
	swordMaterial = swordMat.Object;
	ConstructorHelpers::FObjectFinder<UMaterialInterface> bodyMat(TEXT("Material'/Game/Materials/M_Tank.M_Tank'"));
	bodyMaterial = bodyMat.Object;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> swordHandleMesh(TEXT("SkeletalMesh'/Game/MVRCFPS_Assets/MultiTool/sword/sword_handle.sword_handle'"));
	if (swordHandleMesh.Succeeded())
	{
		ConstructorHelpers::FObjectFinder<UBlendSpace1D> blendSpace(TEXT("BlendSpace1D'/Game/MVRCFPS_Assets/MultiTool/sword/SwordTrigger.SwordTrigger'"));
		if (blendSpace.Succeeded())
		{
			rightMesh->OverrideAnimationData(blendSpace.Object);
		}
		rightMesh->SetSkeletalMesh(swordHandleMesh.Object);
	}

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
	bottomAbilityWidget = createWidget(UJumpDashAbility::StaticClass(), EAbilityWidgetAngle::Half, EAbilityWidgetTriggerLocation::Bottom, TEXT("BottomAbilityWidget"), FName(TEXT("Ability_Widget_Bottom")));

	deathComponent->AfterDeathTeleportPosition = FVector(3706.954346, 131.508514, -1396.709106);
}

void AMeleePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	auto instance = UMaterialInstanceDynamic::Create(multiToolMaterial, nullptr);
	auto swordInstance = UMaterialInstanceDynamic::Create(swordMaterial, nullptr);
	auto bodyInstance = UMaterialInstanceDynamic::Create(bodyMaterial, nullptr);
	
	leftMesh->SetMaterial(0, instance);
	rightMesh->SetMaterial(0, swordInstance);
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

void AMeleePlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("LeftTouchpadX", this, &AMeleePlayerCharacter::OnLeftTouchpadX);
	PlayerInputComponent->BindAxis("LeftTouchpadY", this, &AMeleePlayerCharacter::OnLeftTouchpadY);
}

void AMeleePlayerCharacter::OnLeftTouchpadX(float Value)
{
	topAbilityWidget->SetTouchInputX(Value);
	bottomAbilityWidget->SetTouchInputX(Value);
}

void AMeleePlayerCharacter::OnLeftTouchpadY(float Value)
{
	topAbilityWidget->SetTouchInputY(Value);
	bottomAbilityWidget->SetTouchInputY(Value);
}

