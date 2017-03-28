#include "MajorLeagueGladiator.h"
#include "WaveSystemComponent.h"

UWaveSystemComponent::UWaveSystemComponent()
	: enemyCount(0)
{
	SetIsReplicated(true);
}

void UWaveSystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWaveSystemComponent, enemyCount);
}

void UWaveSystemComponent::OnEnemyKilled(ACharacter* KilledCharacter)
{
	ChangeEnemyCount(-1);
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
