// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "PlayerDamageFeedbackComponent.h"
#include "Characters/MlgPlayerCharacter.h"
#include "MlgPlayerController.h"


UPlayerDamageFeedbackComponent::UPlayerDamageFeedbackComponent()
	: hitDurationGlitch(1.f), leftHitDurationGlitch(0.f), transitionDurationGlitch(0.2f)
{
	PrimaryComponentTick.bCanEverTick = true;
	
	//Sound
	static ConstructorHelpers::FObjectFinder<USoundCue> playerDamageSoundCueFinder(TEXT("SoundCue'/Game/MVRCFPS_Assets/Sounds/Sword_Attack_Cue.Sword_Attack_Cue'"));
	sounds.Add(playerDamageSoundCueFinder.Object);
	//https://docs.unrealengine.com/latest/INT/API/Runtime/Engine/Engine/FPostProcessSettings/index.html
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> glitchMaterial(TEXT("MaterialInstanceConstant'/Game/Materials/GlitchEffect/M_Glitch_PostProcess_Inst.M_Glitch_PostProcess_Inst'"));
	postProcessGlitchMaterial = glitchMaterial.Object;
}

void UPlayerDamageFeedbackComponent::BeginPlay()
{
	postProcessComp = GetOwner()->FindComponentByClass<UPostProcessComponent>();
	if (postProcessComp == nullptr)
	{
		UE_LOG(DebugLog, Warning, TEXT("MlgPlayerCharacter: UPostProcessComponent could not be found!"));
	}
	else
	{
		postProcessComp->Settings.SceneFringeIntensity = 4.f;
		postProcessComp->Settings.VignetteIntensity = 0.75f;
	}
}

void UPlayerDamageFeedbackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (leftHitDurationGlitch > 0.f)
	{
		float scaledValue = (transitionDurationGlitch - (1 - leftHitDurationGlitch)) / transitionDurationGlitch;
		postProcessComp->Settings.AddBlendable(postProcessGlitchMaterial, FMath::Clamp(scaledValue, 0.f, 1.f)); //1.f
		postProcessComp->Settings.bOverride_SceneFringeIntensity = true;
		postProcessComp->Settings.bOverride_VignetteIntensity = true;
		
		leftHitDurationGlitch -= DeltaTime;
		if (leftHitDurationGlitch <= 0.f)
		{
			postProcessComp->Settings.RemoveBlendable(postProcessGlitchMaterial);
			postProcessComp->Settings.bOverride_SceneFringeIntensity = false;
			postProcessComp->Settings.bOverride_VignetteIntensity = false;
		}
	}
}

void UPlayerDamageFeedbackComponent::DoPostProcessVisualization()
{
	if (leftHitDurationGlitch <= 0.f)
	{
		leftHitDurationGlitch = hitDurationGlitch;
	}
}

void UPlayerDamageFeedbackComponent::DoRumble()
{
	AMlgPlayerController* controller = Cast<AMlgPlayerController>(Cast<APawn>(GetOwner())->GetController());//owner->GetMlgPlayerController();
	if (controller != nullptr)
	{
		controller->ClientPlayForceFeedback(controller->GetRumbleShortRight(), false, FName("rumbleRight"));
		controller->ClientPlayForceFeedback(controller->GetRumbleShortLeft(), false, FName("rumbleLeft"));
	}
	else
	{
		UE_LOG(DebugLog, Warning, TEXT("UPlayerDamageFeedbackComponent::DoRumble: Could not cast controller to AMlgPlayerController!"));
	}
}

