#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Weakpoint.generated.h"

USTRUCT()
struct FWeakpoint
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category="Weakpoint")
	FName LocationSocketName;

	UPROPERTY(EditAnywhere, Category = "Weakpoint")
	float MaxAffectDistance;

	UPROPERTY(EditAnywhere, Category = "Weakpoint")
	float DamageMultiplier;

	FWeakpoint()
		: LocationSocketName(NAME_None)
		, MaxAffectDistance(0.f)
		, DamageMultiplier(1.f)
	{}
};