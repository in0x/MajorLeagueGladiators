#include "MajorLeagueGladiator.h"
#include "WaveSystemComponent.h"
#include "WaveSystem/WaveSpawnerManagerComponent.h"
#include "MlgGameMode.h"
#include "MlgGameInstance.h"

UWaveSystemComponent::UWaveSystemComponent()
	: remainingEnemiesForWave(0)
	, startWaveNumber(1)
	, initialTimeBeforeStartSeconds(5.0f)
	, timeBetweenWavesSeconds(6.0f)
	, waveState(EWaveState::NotStarted)
{
	SetIsReplicated(true);

	ConstructorHelpers::FObjectFinder<USoundBase> welcome(TEXT("SoundWave'/Game/MVRCFPS_Assets/Sounds/TEMP/Welcome.Welcome'"));
	firstWaveSound = welcome.Object;

	ConstructorHelpers::FObjectFinder<USoundBase> nextWave(TEXT("SoundWave'/Game/MVRCFPS_Assets/Sounds/TEMP/NextWave.NextWave'"));
	beginOfWaveSound = nextWave.Object;

	ConstructorHelpers::FObjectFinder<USoundCue> endWaveCueFinder(TEXT("SoundCue'/Game/MVRCFPS_Assets/Sounds/TEMP/EndWave_Cue.EndWave_Cue'"));
	endWaveSound = endWaveCueFinder.Object;
}

void UWaveSystemComponent::SetFromSavedState(const SavedState& savedState)
{
	if (savedState.isTravelingToGameMap)
	{		
		remainingEnemiesForWave = 0;
	}
	else
	{
		remainingEnemiesForWave = savedState.remainingEnemies;
	}
	waveState = EWaveState::NotStarted;
	startWaveNumber = savedState.startWaveNumber;
	currentWaveNumber = savedState.currentWaveNumber;
	onRep_remainingEnemiesForWave(remainingEnemiesForWave);
	onRep_currentWaveNumber(currentWaveNumber);	
}
void UWaveSystemComponent::WriteIntoSavedState(SavedState& savedState) const
{
	savedState.startWaveNumber = startWaveNumber;
	savedState.currentWaveNumber = currentWaveNumber;
	savedState.remainingEnemies = remainingEnemiesForWave;
}

void UWaveSystemComponent::SetStartWave(int32 WaveNumber)
{
	startWaveNumber = WaveNumber;
}

void UWaveSystemComponent::StartWave()
{
	check(waveState == EWaveState::NotStarted);
	check(GetOwner()->HasAuthority());

	startWaveImpl(startWaveNumber);
}

void UWaveSystemComponent::startNextWave()
{
	check(waveState == EWaveState::BetweemWave);
	startWaveImpl(currentWaveNumber + 1);
}

void UWaveSystemComponent::startWaveImpl(int32 WaveNumber)
{
	waveState = EWaveState::DuringWave;
	
	setWaveNumber(WaveNumber);

	UWaveSpawnerManagerComponent* spawnManager = GetWorld()->GetAuthGameMode()
		->FindComponentByClass<UWaveSpawnerManagerComponent>();

	if (spawnManager == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No spawn manager on Gamemode"));
		return;
	}

	const int32 waveEnemyCount = spawnManager->StartWave(WaveNumber);
	if (waveEnemyCount == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Enemies were spawned for Wave %d."
			"Either the definiton is missing or no enemy could be spawned due to the modifier"), WaveNumber);
		return;
	}

	changeRemainingEnemiesForWave(waveEnemyCount);
}

void UWaveSystemComponent::OnEnemyKilled(ACharacter* KilledCharacter)
{
	if (waveState == EWaveState::DuringWave)
	{
		changeRemainingEnemiesForWave(-1);
	}
}

void UWaveSystemComponent::Stop()
{
	check(GetOwner()->HasAuthority());

	waveState = EWaveState::NotStarted;
	GetWorld()->GetAuthGameMode<AMlgGameMode>()->DestroyAllAi();

	if (nextActionTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(nextActionTimerHandle);
	}
}

bool UWaveSystemComponent::IsRunning() const
{
	return nextActionTimerHandle.IsValid() || remainingEnemiesForWave > 0;
}

void UWaveSystemComponent::changeRemainingEnemiesForWave(int32 ChangeInValue)
{
	setRemainingEnemiesForWave(GetRemainingEnemiesForWave() + ChangeInValue);
}

void UWaveSystemComponent::setRemainingEnemiesForWave(int32 NewRemainingEnemiesForWave)
{
	check(GetOwner()->HasAuthority());	
	check(NewRemainingEnemiesForWave >= 0);

	int32 oldRemainingEnemiesForWave = remainingEnemiesForWave;
	remainingEnemiesForWave = NewRemainingEnemiesForWave;
	fireRemainingEnemiesForWaveChangedDelegates(oldRemainingEnemiesForWave);

	if (NewRemainingEnemiesForWave == 0 && waveState == EWaveState::DuringWave)
	{
		GetWorld()->GetTimerManager().SetTimer(nextActionTimerHandle, this, &UWaveSystemComponent::startNextWave, timeBetweenWavesSeconds);
		waveState = EWaveState::BetweemWave;
	}
}

void UWaveSystemComponent::setWaveNumber(int32 NewWaveNumber)
{
	int32 oldWaveNumber = currentWaveNumber;
	currentWaveNumber = NewWaveNumber;
	onRep_currentWaveNumber(oldWaveNumber);
}

int32 UWaveSystemComponent::GetRemainingEnemiesForWave() const
{
	return remainingEnemiesForWave;
}

void UWaveSystemComponent::onRep_remainingEnemiesForWave(int32 oldremainingEnemiesForWave)
{
	fireRemainingEnemiesForWaveChangedDelegates(oldremainingEnemiesForWave);
}

void UWaveSystemComponent::onRep_currentWaveNumber(int32 oldWaveNumber)
{
	fireWaveNumberChangedDelegates(oldWaveNumber);
}

void UWaveSystemComponent::fireRemainingEnemiesForWaveChangedDelegates(int32 oldremainingEnemiesForWave)
{
	OnRemainingEnemiesForWaveChanged.Broadcast(remainingEnemiesForWave, oldremainingEnemiesForWave);
	if (remainingEnemiesForWave == 0)
	{
		playEndOfWaveEffects();
		OnWaveCleared.Broadcast(currentWaveNumber);
	}
}

void UWaveSystemComponent::fireWaveNumberChangedDelegates(int32 oldWaveNumber)
{
	if (currentWaveNumber == startWaveNumber)
	{
		playFirstWaveEffects();
	}
	else
	{
		playBeginOfWaveEffects();
	}
	OnWaveStarted.Broadcast(currentWaveNumber);
}

void UWaveSystemComponent::playFirstWaveEffects()
{
	UGameplayStatics::PlaySound2D(GetWorld(), firstWaveSound);
}

void UWaveSystemComponent::playBeginOfWaveEffects()
{
	UGameplayStatics::PlaySound2D(GetWorld(), beginOfWaveSound);
}

void UWaveSystemComponent::playEndOfWaveEffects()
{
	UGameplayStatics::PlaySound2D(GetWorld(), endWaveSound);
}

void UWaveSystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWaveSystemComponent, remainingEnemiesForWave);
	DOREPLIFETIME(UWaveSystemComponent, currentWaveNumber);
	DOREPLIFETIME(UWaveSystemComponent, waveState);
}
