// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "PlayerDamageFeedbackComponent.h"


UPlayerDamageFeedbackComponent::UPlayerDamageFeedbackComponent()
	: hitDuration(0.5f), elapsedHitDuration(0.f)
{
	PrimaryComponentTick.bCanEverTick = true;
	
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
	

	//Convert all constant material instances to dynamic ones:
	//https://docs.unrealengine.com/latest/INT/API/Runtime/Engine/Engine/FPostProcessSettings/index.html
	/*for (auto blendable : postProcessComp->Settings.WeightedBlendables.Array)
	{
		UMaterialInstanceDynamic* matInstanceDyn = UMaterialInstanceDynamic::Create(CastChecked<UMaterialInterface>(blendable.Object), GetOwner());
		blendable.Object = matInstanceDyn;
		dynamicMaterialInstances.Add(matInstanceDyn);
	}
*/
}

void UPlayerDamageFeedbackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (elapsedHitDuration > 0.f)
	{
		postProcessComp->Settings.AddBlendable(postProcessGlitchMaterial, 1.f);
		postProcessComp->Settings.bOverride_SceneFringeIntensity = true;
		postProcessComp->Settings.bOverride_VignetteIntensity = true;
		/*
		UE_LOG(DebugLog, Warning, TEXT("UPlayerDamageFeedbackComponent: Blendables %f"), postProcessComp->Settings.WeightedBlendables.Array.Num());
		for (auto blendable : postProcessComp->Settings.WeightedBlendables.Array)
		{
			blendable.Weight = 1.f;
		}*/
		elapsedHitDuration -= DeltaTime;
		if (elapsedHitDuration <= 0.f)
		{
			postProcessComp->Settings.RemoveBlendable(postProcessGlitchMaterial);
			postProcessComp->Settings.bOverride_SceneFringeIntensity = false;
			postProcessComp->Settings.bOverride_VignetteIntensity = false;
		}
	}
}

void UPlayerDamageFeedbackComponent::DoPostProcessVisualization()
{
	elapsedHitDuration = hitDuration;
}