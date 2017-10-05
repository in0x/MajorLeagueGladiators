#pragma once

#include "MenuAction.generated.h"

UENUM(BlueprintType)
namespace EMenuAction
{
	enum Type
	{
		HostGame			UMETA(DisplayName = "HostGame"),
		JoinGame			UMETA(DisplayName = "JoinGame"),
		StartMeleeTutorial	UMETA(DisplayName = "StartMeleeTutorial"),
		StartRangedTutorial UMETA(DisplayName = "StartRangedTutorial"),
		StartGameEasy		UMETA(DisplayName = "StartGameEasy"),
		StartGameMedium		UMETA(DisplayName = "StartGameMedium"),
		StartGameHard		UMETA(DisplayName = "StartGameHard"),
		GoToMainMenu		UMETA(DisplayName = "GoToMainMenu")
	};
}

DECLARE_MULTICAST_DELEGATE_OneParam(FMenuActionDelegate, TEnumAsByte<EMenuAction::Type>);
