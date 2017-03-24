#pragma once 

#include "IChaperoneBounds.h"
#include "../Plugins/Runtime/Steam/SteamVR/Source/SteamVR/Classes/SteamVRChaperoneComponent.h"

class ChaperoneBounds : public IChaperoneBounds
{
public:
	ChaperoneBounds(USteamVRChaperoneComponent* Chaperone)
		: chaperone(Chaperone)
	{
		IModularFeatures::Get().RegisterModularFeature(IChaperoneBounds::GetModularFeatureName(), this);
	}

	virtual ~ChaperoneBounds() 
	{
		IModularFeatures::Get().UnregisterModularFeature(IChaperoneBounds::GetModularFeatureName(), this);
	}

	virtual TArray<FVector> GetTrackingBounds() const override
	{
		return chaperone->GetBounds();
	}

private:
	USteamVRChaperoneComponent* chaperone;
};
