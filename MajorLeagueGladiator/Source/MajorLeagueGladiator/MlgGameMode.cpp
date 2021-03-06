// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGameMode.h"
#include "Characters/MlgPlayerCharacter.h"
#include "MlgPlayerState.h"
#include "MlgGameState.h"
#include "WaveSystem/WaveSpawnerManagerComponent.h"
#include "WaveSystem/WaveSystemComponent.h"
#include "Ai/MlgAICharacter.h"
#include "MlgPlayerController.h"
#include "MlgGameInstance.h"
#include "Menu/MenuActionComponent.h"
#include "Menu/MenuActionWidget.h"
#include "MlgGameSession.h"
#include "ClassSelection.h"
#include "HealthComponent.h"

namespace
{
	const FString PRE_GAME_MAP("/Game/PreGame?listen");
	const FString GAME_MAP("/Game/ScaleRef?listen");
}

AMlgGameMode::AMlgGameMode(const FObjectInitializer& ObjectInitializer)
	: easyStartWave(1)
	, mediumStartWave(11)
	, hardStartWave(21)
	, hostClassSelection(EClassSelection::Ranged)
{
	//DefaultPawnClass = AMlgPlayerCharacter::StaticClass();
	//PlayerControllerClass = AMlgPlayerController::StaticClass();
	PlayerStateClass = AMlgPlayerState::StaticClass();
	GameStateClass = AMlgGameState::StaticClass();
	waveSpawnerManger = ObjectInitializer.CreateDefaultSubobject<UWaveSpawnerManagerComponent>(this, TEXT("WaveSpawnerManager"));
}

TSubclassOf<AGameSession> AMlgGameMode::GetGameSessionClass() const
{
	return AMlgGameSession::StaticClass();
}

void AMlgGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (!GEngine->IsEditor())
	{
		bUseSeamlessTravel = true;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, TEXT("Not enabling seamless travel in editor as it is not supported in PIE."));
	}

	for (TObjectIterator<UMenuActionComponent> iter; iter; ++iter)
	{
		iter->OnMenuActionTriggered.AddUObject(this, &AMlgGameMode::onMenuAction);
	}

	UWorld* world = GetWorld();

	for (TObjectIterator<UMenuActionWidget> iter; iter; ++iter)
	{
		if (iter->GetWorld() != world)
		{
			continue;
		}
		
		iter->OnMenuActionTriggered.AddUObject(this, &AMlgGameMode::onMenuAction);
	}

	UWaveSystemComponent* waveSystemComponent = GameState->FindComponentByClass<UWaveSystemComponent>();
	waveSystemComponent->OnWaveCleared.AddUObject(this, &AMlgGameMode::onWaveCleared);
	
	if (isInRoomOfShame())
	{
		postEnterRoomOfShame();
	}
	else
	{
		enterGameMap();
	}
}

void AMlgGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	FString hostClassParam = UGameplayStatics::ParseOption(Options, TEXT("hostClass"));

	if (hostClassParam == "Melee")
	{
		hostClassSelection = EClassSelection::Melee;
	}
	else 
	{
		hostClassSelection = EClassSelection::Ranged;
	}
}

void AMlgGameMode::enterGameMap()
{
	UWaveSystemComponent* waveSystemComponent = GameState->FindComponentByClass<UWaveSystemComponent>();
	waveSystemComponent->StartWave();
}

void AMlgGameMode::postEnterRoomOfShame()
{
}

bool AMlgGameMode::isInRoomOfShame() const
{
	return CastChecked<UMlgGameInstance>(GetGameInstance())->bIsInRoomOfShame;
}

void AMlgGameMode::setIsInRoomOfShame(bool NewIsInRoomOfShame)
{
	CastChecked<UMlgGameInstance>(GetGameInstance())->bIsInRoomOfShame = NewIsInRoomOfShame;
}

UClass* AMlgGameMode::getUClassFromClassSelection(EClassSelection::Type selection)
{
	UClass* selectedClass = nullptr;

	if (selection == EClassSelection::Ranged)
	{
		selectedClass = rangedClass.Get();
	}
	else
	{
		selectedClass = meleeClass.Get();
	}

	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EClassSelection"), true);
	check(enumPtr);
	checkf(selectedClass, TEXT("Failed to get UClass for %s"), *enumPtr->GetNameByValue((int64)selection).ToString());
	
	return selectedClass;
}

UClass* AMlgGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	// Handle only Player Controllers, as AI is spawned differently
	if (!InController->IsPlayerController())
	{
		return nullptr;
	}

	if (InController->IsLocalPlayerController())
	{
		return getUClassFromClassSelection(hostClassSelection);
	}
	else
	{
		if (hostClassSelection == EClassSelection::Ranged)
		{
			return getUClassFromClassSelection(EClassSelection::Melee);
		}
		else
		{
			return getUClassFromClassSelection(EClassSelection::Ranged);
		}
	}
}

void AMlgGameMode::onWaveCleared(int32 ClearedWave)
{
	if (ClearedWave == finalWave)
	{
		MatchLost();
	}
}

void AMlgGameMode::MatchLost()
{
	UWaveSystemComponent* waveSystemComponent = GameState->FindComponentByClass<UWaveSystemComponent>();
	waveSystemComponent->Stop();
	travelToRoomOfShame();
}

void AMlgGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
}

void AMlgGameMode::travelToRoomOfShame()
{
	setIsInRoomOfShame(true);
	filterOutAiPlayerStates();

	FString mapWithParam = FString::Printf(TEXT("%s?hostClass=%s"), *PRE_GAME_MAP, *getClassParamString());
	GetWorld()->ServerTravel(mapWithParam, true);
}

void AMlgGameMode::DestroyAllAi()
{
	for (TActorIterator<AMlgAICharacter> iter(GetWorld(), AMlgAICharacter::StaticClass()); iter; ++iter)
	{
		iter->Destroy();
	}
}

void AMlgGameMode::KillAllAi()
{
	for (TActorIterator<AMlgAICharacter> iter(GetWorld(), AMlgAICharacter::StaticClass()); iter; ++iter)
	{
		UHealthComponent* health = iter->FindComponentByClass<UHealthComponent>();
		health->DecreaseHealth(health->GetMaxHealth());
	}
}

void AMlgGameMode::filterOutAiPlayerStates()
{
	GameState->PlayerArray.RemoveAllSwap([](APlayerState* playerState)
	{
		return  playerState == nullptr || playerState->bIsABot != 0;
	});
}

void AMlgGameMode::onMenuAction(TEnumAsByte<EMenuAction::Type> menuAction)
{
	switch (menuAction)
	{
	case EMenuAction::GoToMainMenu:
	{
		travelToMainMenu();
		break;
	}
	case EMenuAction::StartGameEasy:
	{
		beginMatch(easyStartWave);
		break;
	}
	case EMenuAction::StartGameMedium:
	{
		beginMatch(mediumStartWave);
		break;
	}
	case EMenuAction::StartGameHard:
	{
		beginMatch(hardStartWave);
		break;
	}
	default:
	{
		checkNoEntry();
		break;
	}
	}
}

void AMlgGameMode::travelToMainMenu()
{
	UWaveSystemComponent* waveSystemComponent = GameState->FindComponentByClass<UWaveSystemComponent>();
	waveSystemComponent->Stop();
	CastChecked<UMlgGameInstance>(GetGameInstance())->TravelToMainMenu();
}

void AMlgGameMode::beginMatch(int32 StartWave)
{
	if (isInRoomOfShame())
	{
		UWaveSystemComponent* waveSystemComponent = GameState->FindComponentByClass<UWaveSystemComponent>();
		waveSystemComponent->SetStartWave(StartWave);
		travelToGameMap();
	}
}

void AMlgGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
}

void AMlgGameMode::travelToGameMap()
{
	setIsInRoomOfShame(false);

	FString mapWithParam = FString::Printf(TEXT("%s?hostClass=%s"), *GAME_MAP, *getClassParamString());

	GetWorld()->ServerTravel(mapWithParam, true);
}

FString AMlgGameMode::getClassParamString() const
{
	if (hostClassSelection == EClassSelection::Ranged)
	{
		return FString("Ranged");
	}
	else
	{
		return FString("Melee");
	}
}


