#include "MajorLeagueGladiator.h"
#include "WaveSystemComponent.h"
#include "WaveSystem/WaveSpawnerManagerComponent.h"

UWaveSystemComponent::UWaveSystemComponent()
	: remainingEnemiesForWave(0)
	, startWaveNumber(1)
	, initialTimeBeforeStartSeconds(2.0f)
	, timeBetweenWavesSeconds(2.0f)
{
	SetIsReplicated(true);
}

void UWaveSystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWaveSystemComponent, remainingEnemiesForWave);
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
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Emerald, FString::Printf(TEXT("Beginn Spawning Wave %d"), WaveNumber));
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

	currentWaveNumber = WaveNumber;
	UWaveSpawnerManagerComponent* spawnManager = GetWorld()->GetAuthGameMode()
		->FindComponentByClass<UWaveSpawnerManagerComponent>();

	if (spawnManager == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No spawn manager on Gamemode"));
		return;
	}

	const int32 spawnendEnemies = spawnManager->StartWave(WaveNumber);
	if (spawnendEnemies == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Enemies were spawned for Wave %d."
			"Either the definiton is missing or no enemy could be spawned due to the modifier"), WaveNumber);
		return;
	}

	ChangeRemainingEnemiesForWave(spawnendEnemies);
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

	int32 oldRemainingEnemiesForWave = remainingEnemiesForWave;
	remainingEnemiesForWave = NewRemainingEnemiesForWave;
	if (remainingEnemiesForWave == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Emerald, FString::Printf(TEXT("Wave %d has been defeated"), currentWaveNumber));
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, this, &UWaveSystemComponent::StartNextWave, timeBetweenWavesSeconds);	
	}
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

void UWaveSystemComponent::fireRemainingEnemiesForWaveChangedDelegates(int32 oldremainingEnemiesForWave)
{
	OnRemainingEnemiesForWaveChanged.Broadcast(remainingEnemiesForWave, oldremainingEnemiesForWave);
	if (remainingEnemiesForWave > oldremainingEnemiesForWave)
	{
		OnWaveStarted.Broadcast(currentWaveNumber);
	}
	else if (remainingEnemiesForWave == 0)
	{
		OnWaveCleared.Broadcast(currentWaveNumber);
	}
}

