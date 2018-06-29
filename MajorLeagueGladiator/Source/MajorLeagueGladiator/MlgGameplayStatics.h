// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmitterSpawnParams.h"
#include "BeamEmitterSpawnParams.h"
#include "SoundParams.h"
#include "VRExpansion/VRBPDatatypes.h"
#include "MlgGameplayStatics.generated.h"

class AMlgPlayerState;

UCLASS()
class MAJORLEAGUEGLADIATOR_API UMlgGameplayStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	static bool CanDealDamageTo(const AActor* DamageDealer, const AActor* DamageReceiver);
	static bool CanDealDamageTo(const APawn* DamageInstigator, const AActor* DamageReceiver);
	static bool CanDealDamageTo(const AController* DamageInstigator, const AActor* DamageReceiver);
	static bool CanDealDamageTo(const AMlgPlayerState* DamageDealerState, const AActor* DamageReceiver);
	static bool CanDealDamageTo(const AMlgPlayerState* DamageDealer, const AMlgPlayerState* DamageReceiver);
	static bool ComponentIsDamageableFrom(UPrimitiveComponent* VictimComp, FVector const& Origin, AActor const* IgnoredActor, const TArray<AActor*>& IgnoreActors, ECollisionChannel TraceChannel, FHitResult& OutHitResult);
	static bool ApplyRadialDamageWithFalloff(const UObject* WorldContextObject, float BaseDamage, float MinimumDamage, const FVector& Origin, float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff, TSubclassOf<class UDamageType> DamageTypeClass, AActor* DamageCauser, AController* InstigatedByController, bool bDrawDebug = false, const TArray<AActor*>& IgnoreActors = TArray<AActor*>(), ECollisionChannel DamagePreventionChannel = ECC_Visibility);
	static void ApplyPointDamageNetworked(AActor* DamagedActor, float BaseDamage, const FVector& HitFromDirection, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<class UDamageType> DamageTypeClass);

	static UParticleSystemComponent* SpawnBeamEmitter(UWorld* World, const FBeamEmitterSpawnParams& Params);

	static void SpawnEmitterNetworked(UWorld* World, const FEmitterSpawnParams& Params);
	static void SpawnEmitterNetworkedPredicted(const APawn* Source, const FEmitterSpawnParams& Params);
	static void SpawnEmitterLocalOnly(const APawn* Source, const FEmitterSpawnParams& Params);

	static void SpawnBeamEmitterNetworked(UWorld* World, const FBeamEmitterSpawnParams& Params);
	static void SpawnBeamEmitterNetworkedPredicted(const APawn* Source, const FBeamEmitterSpawnParams& Params);
	static void SpawnBeamEmitterLocalOnly(const APawn* Source, const FBeamEmitterSpawnParams& Params);

	static void PlaySoundNetworked(UWorld* World, const FSoundParams& Params);
	static void PlaySoundNetworkedPredicted(const APawn* Source, const FSoundParams& Params);
	static void PlaySoundLocalOnly(const APawn* Source, const FSoundParams& Params);

	static bool IsUsingDeviceOfType(EHMDDeviceType::Type DeviceType);

	UFUNCTION(BlueprintCallable, Category = "MlgGameplayStaticsFunction", meta = (DisplayName = "IsUsingDeviceOfTypeBP"))
	static bool IsUsingDeviceOfTypeBP(EBPHMDDeviceType DeviceType);
};
