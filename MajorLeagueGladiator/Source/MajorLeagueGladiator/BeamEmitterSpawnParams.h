#pragma once

#include "CoreMinimal.h"
#include "BeamEmitterSpawnParams.generated.h"

class UParticleSystem;

USTRUCT()
struct FBeamEmitterSpawnParams
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	UParticleSystem* Template;

	UPROPERTY()
	FVector Source;

	UPROPERTY()
	FVector Target;

	UPROPERTY()
		bool bAutoDestroy;

	FBeamEmitterSpawnParams()
		: Template(nullptr)
		, Source(FVector::ZeroVector)
		, Target(FVector::ZeroVector)
		, bAutoDestroy(true)
	{}

	explicit FBeamEmitterSpawnParams(UParticleSystem* psTemplate, FVector Source, FVector Target, bool psAutoDestroy = true)
		: Template(psTemplate)
		, Source(Source)
		, Target(Target)
		, bAutoDestroy(psAutoDestroy)
	{}
};
