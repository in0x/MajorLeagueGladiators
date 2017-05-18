// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "PlayerDamageFeedbackComponent.h"


UPlayerDamageFeedbackComponent::UPlayerDamageFeedbackComponent()
	: hitDuration(0.5f), elapsedHitDuration(0.f)
{

}

void UPlayerDamageFeedbackComponent::BeginPlay()
{
	postProcessComp = GetOwner()->FindComponentByClass<UPostProcessComponent>();
	if (postProcessComp == nullptr)
	{
		UE_LOG(DebugLog, Warning, TEXT("MlgPlayerCharacter: UPostProcessComponent could not be found!"));
	}

	//Convert all constant material instances to dynamic ones:
	for (auto blendable : postProcessComp->Settings.WeightedBlendables.Array)
	{
		UMaterialInstanceDynamic* matInstanceDyn = UMaterialInstanceDynamic::Create(CastChecked<UMaterialInterface>(blendable.Object), GetOwner());
		blendable.Object = matInstanceDyn;
	}

}

void UPlayerDamageFeedbackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (elapsedHitDuration > 0.f)
	{
		for (auto blendable : postProcessComp->Settings.WeightedBlendables.Array)
		{
			blendable.Weight = 1.f;
		}
		elapsedHitDuration -= DeltaTime;
		/*if (elapsedHitDuration <= 0.f)
		{
			for (auto blendable : postProcessComp->Settings.WeightedBlendables.Array)
			{
				blendable.Weight = 0.f;
			}
		}*/
	}
}

void UPlayerDamageFeedbackComponent::DoPostProcessVisualization()
{
	elapsedHitDuration = hitDuration;
}