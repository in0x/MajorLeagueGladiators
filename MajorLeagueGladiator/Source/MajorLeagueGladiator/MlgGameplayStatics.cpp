// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MlgGameplayStatics.h"
#include "MlgPlayerState.h"
#include "ReplicatedEffectsComponent.h"
#include "PredictedEffectsComponent.h"

namespace
{
	UReplicatedEffectsComponent* getReplicatedEffectsComponent(UWorld* World)
	{
		AGameStateBase* gameState = World->GetGameState();
		check(gameState);
		UReplicatedEffectsComponent* repFxComp = gameState->FindComponentByClass<UReplicatedEffectsComponent>();
		check(repFxComp);
		return repFxComp;
	}

	UPredictedEffectsComponent* getPredictedEffectsComponent(const APawn* Source)
	{
		UPredictedEffectsComponent* predictEffectComp = Source->FindComponentByClass<UPredictedEffectsComponent>();
		check(predictEffectComp);
		return predictEffectComp;
	}

	FName BEAM_EMITTER_TARGET_NAME("BeamTarget");

}


bool UMlgGameplayStatics::CanDealDamageTo(const AActor* DamageDealer, const AActor* DamageReceiver)
{
	check(DamageDealer);
	check(DamageReceiver);

	const APawn* DamageDealerPawn = Cast<APawn>(DamageDealer);
	if (DamageDealerPawn == nullptr)
	{
		DamageDealerPawn = DamageDealer->GetInstigator();
	}

	if (DamageDealerPawn == nullptr)
	{
		UE_LOG(DebugLog, Warning, TEXT("Damage Dealer Pawn has no instigator"));
		return false;
	}

	return CanDealDamageTo(DamageDealerPawn, DamageReceiver);
}

bool UMlgGameplayStatics::CanDealDamageTo(const APawn* DamageInstigator, const AActor* DamageReceiver)
{
	check(DamageInstigator);
	check(DamageReceiver);

	if (DamageInstigator->PlayerState == nullptr)
	{
		UE_LOG(DebugLog, Warning, TEXT(" \"%s\" has no PlayerState"), *DamageInstigator->GetName());
		return false;
	}

	const AMlgPlayerState* playerstate = CastChecked<AMlgPlayerState>(DamageInstigator->PlayerState);
	return playerstate && CanDealDamageTo(playerstate, DamageReceiver);
}

bool UMlgGameplayStatics::CanDealDamageTo(const AController* DamageInstigator, const AActor* DamageReceiver)
{	
	check(DamageInstigator);
	check(DamageReceiver);

	const AMlgPlayerState* DamageInstigatorState = CastChecked<AMlgPlayerState>(DamageInstigator->PlayerState);
	return CanDealDamageTo(DamageInstigatorState, DamageReceiver);
}

bool UMlgGameplayStatics::CanDealDamageTo(const AMlgPlayerState* DamageDealerState, const AActor* DamageReceiver)
{
	check(DamageDealerState);
	check(DamageReceiver);

	if (const APawn* DamageReceiverPawn = Cast<APawn>(DamageReceiver))
	{
		if (DamageReceiverPawn->PlayerState == nullptr)
		{
			// Can damage stuff without playerstates
			return false;
		}
		const AMlgPlayerState* DamageReceiverState = CastChecked<AMlgPlayerState>(DamageReceiverPawn->PlayerState);
		return CanDealDamageTo(DamageDealerState, DamageReceiverState);
	}

	// Forbid damaging non pawns,
	// we might want to change this in the future
	return false;
}

bool UMlgGameplayStatics::CanDealDamageTo(const AMlgPlayerState* DamageDealer, const AMlgPlayerState* DamageReceiver)
{
	check(DamageDealer);
	check(DamageReceiver);

	return !DamageDealer->IsSameTeam(DamageReceiver);
}

bool UMlgGameplayStatics::ComponentIsDamageableFrom(UPrimitiveComponent* VictimComp, FVector const& Origin, AActor const* IgnoredActor, const TArray<AActor*>& IgnoreActors, ECollisionChannel TraceChannel, FHitResult& OutHitResult)
{
	static FName NAME_ComponentIsVisibleFrom = FName(TEXT("ComponentIsVisibleFrom"));
	FCollisionQueryParams lineParams(NAME_ComponentIsVisibleFrom, true, IgnoredActor);
	lineParams.AddIgnoredActors(IgnoreActors);

	// Do a trace from origin to middle of box
	UWorld* const world = VictimComp->GetWorld();
	
	const FVector traceEnd = VictimComp->Bounds.Origin;
	FVector traceStart = Origin;
	if (Origin == traceEnd)
	{
		// tiny nudge so LineTraceSingle doesn't early out with no hits
		traceStart.Z += 0.01f;
	}
	
	const bool bHadBlockingHit = world->LineTraceSingleByChannel(OutHitResult, traceStart, traceEnd, TraceChannel, lineParams);
	
	// If there was a blocking hit, it will be the last one
	if (bHadBlockingHit)
	{
		if (OutHitResult.Component == VictimComp)
		{
			// if blocking hit was the victim component, it is visible
			return true;
		}
		else
		{
			// if we hit something else blocking, it's not
			UE_LOG(LogDamage, Log, TEXT("Radial Damage to %s blocked by %s (%s)"), *GetNameSafe(VictimComp), *GetNameSafe(OutHitResult.GetActor()), *GetNameSafe(OutHitResult.Component.Get()));
			return false;
		}
	}

	// didn't hit anything, assume nothing blocking the damage and victim is consequently visible
	// but since we don't have a hit result to pass back, construct a simple one, modeling the damage as having hit a point at the component's center.
	FVector const fakeHitLoc = VictimComp->GetComponentLocation();
	FVector const fakeHitNorm = (Origin - fakeHitLoc).GetSafeNormal();		// normal points back toward the epicenter
	OutHitResult = FHitResult(VictimComp->GetOwner(), VictimComp, fakeHitLoc, fakeHitNorm);
	return true;
}


bool UMlgGameplayStatics::ApplyRadialDamageWithFalloff(const UObject* WorldContextObject, float BaseDamage, float MinimumDamage, const FVector& Origin, 
	float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff, TSubclassOf<class UDamageType> DamageTypeClass, AActor* DamageCauser, 
	AController* InstigatedByController, bool bDrawDebug, const TArray<AActor*>& IgnoreActors, ECollisionChannel DamagePreventionChannel)
{
	static FName NAME_ApplyRadialDamage = FName(TEXT("ApplyRadialDamage"));
	FCollisionQueryParams sphereParams(NAME_ApplyRadialDamage, false, DamageCauser);

	sphereParams.AddIgnoredActors(IgnoreActors);

	// query scene to see what we hit
	TArray<FOverlapResult> overlaps;
	UWorld* world = GEngine->GetWorldFromContextObject(WorldContextObject);
	world->OverlapMultiByObjectType(
		overlaps, 
		Origin, 
		FQuat::Identity, 
		FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), 
		FCollisionShape::MakeSphere(DamageOuterRadius), 
		sphereParams);

	// collate into per-actor list of hit components
	TMap<AActor*, TArray<FHitResult>> overlapComponentMap;
	int32 count = overlaps.Num();
	for (int32 i = 0; i < count; ++i)
	{
		FOverlapResult const& Overlap = overlaps[i];
		AActor* const OverlapActor = Overlap.GetActor();

		if (OverlapActor &&
			OverlapActor->bCanBeDamaged &&
			OverlapActor != DamageCauser &&
			Overlap.Component.IsValid() &&
			CanDealDamageTo(DamageCauser, OverlapActor))
		{
			FHitResult Hit;
			if (DamagePreventionChannel == ECC_MAX || 
				ComponentIsDamageableFrom(Overlap.Component.Get(), Origin, DamageCauser, IgnoreActors, DamagePreventionChannel, Hit))
			{
				TArray<FHitResult>& HitList = overlapComponentMap.FindOrAdd(OverlapActor);
				HitList.Add(Hit);
			}
		}
	}

	// make sure we have a good damage type
	TSubclassOf<UDamageType> const validDamageTypeClass = DamageTypeClass ? DamageTypeClass : TSubclassOf<UDamageType>(UDamageType::StaticClass());

	bool bAppliedDamage = false;

	// call damage function on each affected actors
	for (auto& pair : overlapComponentMap)
	{
		AActor* const victim = pair.Key;
		TArray<FHitResult> const& componentHits = pair.Value;

		FRadialDamageEvent dmgEvent;
		dmgEvent.DamageTypeClass = validDamageTypeClass;
		dmgEvent.ComponentHits = componentHits;
		dmgEvent.Origin = Origin;
		dmgEvent.Params = FRadialDamageParams(BaseDamage, MinimumDamage, DamageInnerRadius, DamageOuterRadius, DamageFalloff);

		victim->TakeDamage(BaseDamage, dmgEvent, InstigatedByController, DamageCauser);

		bAppliedDamage = true;
	}

	if (bDrawDebug)
	{
		DrawDebugSphere(world, Origin, DamageOuterRadius, 30, FColor::Magenta, false, 5.0f);

		for (const FOverlapResult& overlap : overlaps)
		{
			DrawDebugSphere(world, overlap.Actor->GetActorLocation(), 20.0f, 10, FColor::Red, false, 5.f);
		}
	}

	return bAppliedDamage;
}


void UMlgGameplayStatics::ApplyPointDamageNetworked(AActor* DamagedActor, float BaseDamage, const FVector& HitFromDirection, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<class UDamageType> DamageTypeClass)
{
	check(CanDealDamageTo(EventInstigator, DamagedActor));
	getReplicatedEffectsComponent(EventInstigator->GetWorld())->ApplyPointDamage_Server(DamagedActor, BaseDamage, HitFromDirection, HitInfo, EventInstigator, DamageCauser, DamageTypeClass);
}

UParticleSystemComponent* UMlgGameplayStatics::SpawnBeamEmitter(UWorld* World, const FBeamEmitterSpawnParams& Params)
{
	UParticleSystemComponent* PSC = nullptr;
	if (World && Params.Template)
	{
		PSC = UGameplayStatics::SpawnEmitterAtLocation(World, Params.Template, Params.Source, (Params.Target - Params.Source).Rotation(), Params.bAutoDestroy);
		PSC->SetVectorParameter(BEAM_EMITTER_TARGET_NAME, Params.Target);
	}
	return PSC;
}

void UMlgGameplayStatics::SpawnEmitterNetworked(UWorld* World, const FEmitterSpawnParams& Params)
{
	getReplicatedEffectsComponent(World)->CreateParticleSystemAtLocation(Params);
}

void UMlgGameplayStatics::SpawnEmitterNetworkedPredicted(const APawn* Source, const FEmitterSpawnParams& Params)
{
	getPredictedEffectsComponent(Source)->CreateParticleSystemNetworkedPredicted(Params);
}

void UMlgGameplayStatics::SpawnEmitterLocalOnly(const APawn* Source, const FEmitterSpawnParams& Params)
{
	getPredictedEffectsComponent(Source)->CreateParticleSystemLocal(Params);
}



void UMlgGameplayStatics::SpawnBeamEmitterNetworked(UWorld* World, const FBeamEmitterSpawnParams& Params)
{
	getReplicatedEffectsComponent(World)->CreateBeamParticleSystemAtLocation(Params);
}

void UMlgGameplayStatics::SpawnBeamEmitterNetworkedPredicted(const APawn* Source, const FBeamEmitterSpawnParams& Params)
{
	getPredictedEffectsComponent(Source)->CreateBeamParticleSystemNetworkedPredicted(Params);
}

void UMlgGameplayStatics::SpawnBeamEmitterLocalOnly(const APawn* Source, const FBeamEmitterSpawnParams& Params)
{
	getPredictedEffectsComponent(Source)->CreateBeamParticleSystemLocal(Params);
}




void UMlgGameplayStatics::PlaySoundNetworked(UWorld* World, const FSoundParams& Params)
{
	getReplicatedEffectsComponent(World)->PlaySoundNetworked(Params);
}

void UMlgGameplayStatics::PlaySoundNetworkedPredicted(const APawn* Source, const FSoundParams& Params)
{
	getPredictedEffectsComponent(Source)->PlaySoundNetworkedPredicted(Params);
}

void UMlgGameplayStatics::PlaySoundLocalOnly(const APawn* Source, const FSoundParams& Params)
{
	getPredictedEffectsComponent(Source)->PlaySoundLocal(Params);
}

bool UMlgGameplayStatics::IsUsingDeviceOfType(EHMDDeviceType::Type DeviceType)
{
#define HACK_FORCE_OCULUS 0

#if HACK_FORCE_OCULUS
	return DeviceType == EHMDDeviceType::DT_OculusRift;
#else
	return GEngine
		&& g_IsVREnabled()
		&& (GEngine->XRSystem->GetHMDDevice()->GetHMDDeviceType() == DeviceType);
#endif
}

namespace
{
	EHMDDeviceType::Type BpHmdTypeToEHmdType(EBPHMDDeviceType HMDDeviceType)
	{
		switch (HMDDeviceType)
		{
		case EBPHMDDeviceType::DT_ES2GenericStereoMesh: return EHMDDeviceType::DT_ES2GenericStereoMesh; 
		case EBPHMDDeviceType::DT_GearVR: return EHMDDeviceType::DT_GearVR; 
		case EBPHMDDeviceType::DT_Morpheus: return EHMDDeviceType::DT_Morpheus; 
		case EBPHMDDeviceType::DT_OculusRift: return EHMDDeviceType::DT_OculusRift; 
		case EBPHMDDeviceType::DT_SteamVR: return EHMDDeviceType::DT_SteamVR; 
		case EBPHMDDeviceType::DT_GoogleVR: return EHMDDeviceType::DT_GoogleVR; 
		default: checkNoEntry(); return EHMDDeviceType::DT_ES2GenericStereoMesh; 
		}
	}
}

bool UMlgGameplayStatics::IsUsingDeviceOfTypeBP(EBPHMDDeviceType DeviceType)
{
	return IsUsingDeviceOfType(::BpHmdTypeToEHmdType(DeviceType));
}



