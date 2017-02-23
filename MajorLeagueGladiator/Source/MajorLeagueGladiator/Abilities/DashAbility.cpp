#include "MajorLeagueGladiator.h"
#include "DashAbility.h"
#include "MlgPlayerCharacter.h"
#include "VRControllerComponent.h"
#include "AbilityTask_WaitTargetData.h"
#include "GameplayAbilityTargetActor_PredictProjectile.h"

UDashAbility::UDashAbility()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	bReplicateInputDirectly = true;
}
