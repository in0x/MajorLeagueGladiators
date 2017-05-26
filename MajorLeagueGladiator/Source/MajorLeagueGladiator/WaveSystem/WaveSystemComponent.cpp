#include "MajorLeagueGladiator.h"
#include "WaveSystemComponent.h"
#include "WaveSystem/WaveSpawnerManagerComponent.h"

UWaveSystemComponent::UWaveSystemComponent()
	: remainingEnemiesForWave(0)
	, startWaveNumber(1)
	, initialTimeBeforeStartSeconds(5.0f)
	, timeBetweenWavesSeconds(6.0f)
{
	SetIsReplicated(true);

	ConstructorHelpers::FObjectFinder<USoundBase> welcome(TEXT("SoundWave'/Game/MVRCFPS_Assets/Sounds/TEMP/Welcome.Welcome'"));
	firstWaveSound = welcome.Object;

	ConstructorHelpers::FObjectFinder<USoundBase> nextWave(TEXT("SoundWave'/Game/MVRCFPS_Assets/Sounds/TEMP/NextWave.NextWave'"));
	beginOfWaveSound = nextWave.Object;

	ConstructorHelpers::FObjectFinder<USoundCue> endWaveCueFinder(TEXT("SoundCue'/Game/MVRCFPS_Assets/Sounds/TEMP/EndWave_Cue.EndWave_Cue'"));
	endWaveCue = endWaveCueFinder.Object;
}

void UWaveSystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWaveSystemComponent, remainingEnemiesForWave);
	DOREPLIFETIME(UWaveSystemComponent, currentWaveNumber);
}

void UWaveSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner()->HasAuthority())
	{
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, this, &UWaveSystemComponent::Start, initialTimeBeforeStartSeconds);
	}
}

void UWaveSystemComponent::Start()
{
	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Can not spawn wave on client"));
		return;
	}
	StartWave(startWaveNumber);
}

void UWaveSystemComponent::OnEnemyKilled(ACharacter* KilledCharacter)
{
	ChangeRemainingEnemiesForWave(-1);
}

void UWaveSystemComponent::StartNextWave()
{
	StartWave(currentWaveNumber + 1);
}

void UWaveSystemComponent::StartWave(int32 WaveNumber)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Emerald, FString::Printf(TEXT("Beginn Spawning Wave %d"), WaveNumber));
	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Can not spawn wave on client"));
		return;
	}

	if (GetRemainingEnemiesForWave() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can not spawn wave while wave is still in progress"));
		return;
	}

	int32 oldWaveNumber = currentWaveNumber;
	currentWaveNumber = WaveNumber;
	fireWaveNumberChangedDelegates(oldWaveNumber);

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

	ChangeRemainingEnemiesForWave(waveEnemyCount);
}

void UWaveSystemComponent::ChangeRemainingEnemiesForWave(int32 ChangeInValue)
{
	SetRemainingEnemiesForWave(GetRemainingEnemiesForWave() + ChangeInValue);
}

void UWaveSystemComponent::SetRemainingEnemiesForWave(int32 NewRemainingEnemiesForWave)
{
	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Can not change remainingEnemiesForWave on client"));
		return;
	}
	check(NewRemainingEnemiesForWave >= 0);
	
	if (NewRemainingEnemiesForWave == 0)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Emerald, FString::Printf(TEXT("Wave %d has been defeated"), currentWaveNumber));
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, this, &UWaveSystemComponent::StartNextWave, timeBetweenWavesSeconds);	
	}

	int32 oldRemainingEnemiesForWave = remainingEnemiesForWave;
	remainingEnemiesForWave = NewRemainingEnemiesForWave;
	fireRemainingEnemiesForWaveChangedDelegates(oldRemainingEnemiesForWave);
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
	UGameplayStatics::PlaySound2D(GetWorld(), endWaveCue);
}

