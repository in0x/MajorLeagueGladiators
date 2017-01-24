#pragma once

#include "TriggerZoneComponent.h"
#include "Usable.generated.h"

UINTERFACE()
class PROTOTYPE_API UUsable : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class PROTOTYPE_API IUsable
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION()
	virtual void Use(AActor* User, TriggerType Type) = 0;
};