#pragma once

#include "Engine.h"
#include "IMotionController.h"

class APlayerCharacter;

class IPlayerCharacterMotionController : public IMotionController
{
public:

	static FName GetModularFeatureName()
	{
		static FName FeatureName = FName(TEXT("IPlayerCharacterContainer"));
		return FeatureName;
	}

	virtual void SetPlayerCharacter(APlayerCharacter*) = 0;
};