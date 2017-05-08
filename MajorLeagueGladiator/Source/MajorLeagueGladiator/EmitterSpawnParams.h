#pragma once

#include "CoreMinimal.h"
#include "EmitterSpawnParams.generated.h"

class UParticleSystem;

USTRUCT()
struct FEmitterSpawnParams
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	UParticleSystem* Template;

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	bool bAutoDestroy;

	FEmitterSpawnParams()
		: Template(nullptr)
		, Transform(FTransform::Identity)
		, bAutoDestroy(true)
	{}

	explicit FEmitterSpawnParams(UParticleSystem* psTemplate ,const FTransform& psTransform = FTransform::Identity, bool psAutoDestroy = true)
		: Template(psTemplate)
		, Transform(psTransform)
		, bAutoDestroy(psAutoDestroy)
	{}
};