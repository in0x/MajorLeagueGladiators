#include "MajorLeagueGladiator.h"
#include "WaveSystemComponent.h"
#include "WaveSystem/WaveSpawnerManagerComponent.h"

UWaveSystemComponent::UWaveSystemComponent()
	: enemyCount(0)
	, startWaveNumber(1)
	, initialTimeBeforeStartSeconds(2.0f)
	, timeBetweenWavesSeconds(2.0f)
{
	SetIsReplicated(true);
}

void UWaveSystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWaveSystemComponent, enemyCount);
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
	ChangeEnemyCount(-1);
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
	if (GetEnemyCount() > 0)
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
			"Either the definiton is missing or no enemy could be spawned due to the modifier"));
		return;
	}

	ChangeEnemyCount(spawnendEnemies);
}

void UWaveSystemComponent::ChangeEnemyCount(int32 ChangeInValue)
{
	SetEnemyCount(GetEnemyCount() + ChangeInValue);
}

void UWaveSystemComponent::SetEnemyCount(int32 NewEnemyCount)
{
	int32 oldEnemyCount = enemyCount;
	enemyCount = NewEnemyCount;
	OnEnemyCountChanged.Broadcast(enemyCount, oldEnemyCount);
	if (enemyCount == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Emerald, FString::Printf(TEXT("Wave %d has been defeated"), currentWaveNumber));
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, this, &UWaveSystemComponent::StartNextWave, timeBetweenWavesSeconds);
		OnEnemyCountZero.Broadcast();
	}
}

int32 UWaveSystemComponent::GetEnemyCount() const
{
	return enemyCount;
}

void UWaveSystemComponent::onRep_EnemyCount(int32 oldEnemyCount)
{
	OnEnemyCountChanged.Broadcast(enemyCount, oldEnemyCount);
	if (enemyCount == 0)
	{
		OnEnemyCountZero.Broadcast();
	}
}
