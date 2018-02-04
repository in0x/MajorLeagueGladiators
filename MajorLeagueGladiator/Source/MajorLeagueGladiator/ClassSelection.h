#pragma once

#include "ClassSelection.generated.h"

UENUM(BlueprintType)
namespace EClassSelection
{
	enum Type
	{
		Ranged			UMETA(DisplayName = "Ranged"),
		Melee			UMETA(DisplayName = "Melee"),
	};
}
