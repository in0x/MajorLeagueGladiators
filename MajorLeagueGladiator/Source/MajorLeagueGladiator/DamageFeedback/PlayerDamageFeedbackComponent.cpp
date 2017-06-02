// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "PlayerDamageFeedbackComponent.h"
#include "Characters/MlgPlayerCharacter.h"
#include "MlgPlayerController.h"


UPlayerDamageFeedbackComponent::UPlayerDamageFeedbackComponent()
	: hitDurationGlitch(0.3f)
	, leftHitDurationGlitch(0.f)
	, transitionDurationGlitch(0.1f)
{
	PrimaryComponentTick.bCanEverTick = true;
	
	//Sound
	static ConstructorHelpers::FObjectFinder<USoundCue> playerDamageSoundCueFinder(TEXT("SoundCue'/Game/MVRCFPS_Assets/Sounds/Damage_Taken_16bit_Cue.Damage_Taken_16bit_Cue'"));
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
		if (postProcessComp->Settings.SceneFringeIntensity != 0.f || postProcessComp->Settings.VignetteIntensity != 0.4f)
		{
			UE_LOG(DebugLog, Warning, TEXT("UPlayerDamageFeedbackComponent::BeginPlay: Values of post process component are overridden!"));
		}
		postProcessComp->Settings.SceneFringeIntensity = 4.f;
		postProcessComp->Settings.VignetteIntensity = 0.75f;
	}
}

void UPlayerDamageFeedbackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (leftHitDurationGlitch > 0.f)
	{
		leftHitDurationGlitch -= DeltaTime;
		if (leftHitDurationGlitch <= 0.f)
		{
			postProcessComp->Settings.RemoveBlendable(postProcessGlitchMaterial);
			postProcessComp->Settings.bOverride_SceneFringeIntensity = false;
			postProcessComp->Settings.bOverride_VignetteIntensity = false;
		}
	}
}

void UPlayerDamageFeedbackComponent::DoGlitchEffect()
{
	if (leftHitDurationGlitch <= 0.f)
	{
		leftHitDurationGlitch = hitDurationGlitch;
		
		postProcessComp->Settings.AddBlendable(postProcessGlitchMaterial, 1.f); //OLD: 0.f
		postProcessComp->Settings.bOverride_SceneFringeIntensity = true;
		postProcessComp->Settings.bOverride_VignetteIntensity = true;
	}
}

void UPlayerDamageFeedbackComponent::DoRumble()
{
	if (AMlgPlayerCharacter* player = Cast<AMlgPlayerCharacter>(GetOwner()))
	{
		player->PlayRumbleRight();
		player->PlayRumbleLeft();
	}
	else
	{
		UE_LOG(DebugLog, Warning, TEXT("UPlayerDamageFeedbackComponent::DoRumble: Could not cast controller to AMlgPlayerController!"));
	}
}

