#pragma once

#include "CoreMinimal.h"
#include "SoundParams.generated.h"

class USoundBase;
class USoundAttenuation;
class USoundConcurrency;

USTRUCT()
struct FSoundParams
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	USoundBase* Sound;

	UPROPERTY()
	USoundAttenuation* AttenuationSettings;

	UPROPERTY()
	USoundConcurrency* ConcurrencySettings;

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FRotator Rotation;

	UPROPERTY()
	float VolumeMultiplier;

	UPROPERTY()
	float PitchMultiplier;

	UPROPERTY()
	float StartTime;

	FSoundParams()
		: FSoundParams(nullptr, FVector::ZeroVector)
	{}

	FSoundParams(USoundBase* sound, const FVector& location = FVector::ZeroVector)
		: Sound(sound)
		, AttenuationSettings(nullptr)
		, ConcurrencySettings(nullptr)
		, Location(location)
		, Rotation(FRotator::ZeroRotator)
		, VolumeMultiplier(1.0f)
		, PitchMultiplier(1.0f)
		, StartTime(0.0f)
	{}
};
