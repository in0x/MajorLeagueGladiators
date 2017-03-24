#pragma once

#include "CoreMinimal.h"
#include "Features/IModularFeature.h"

class IChaperoneBounds : public IModularFeature
{
public:

	static FName GetModularFeatureName()
	{
		static FName FeatureName = FName(TEXT("ChaperoneBounds"));
		return FeatureName;
	}

	virtual TArray<FVector> GetTrackingBounds() const = 0;
};