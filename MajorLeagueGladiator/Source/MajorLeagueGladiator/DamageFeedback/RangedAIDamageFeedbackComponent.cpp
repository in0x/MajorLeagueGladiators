// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "RangedAIDamageFeedbackComponent.h"


URangedAIDamageFeedbackComponent::URangedAIDamageFeedbackComponent()
{
	static ConstructorHelpers::FObjectFinder<UParticleSystem> hitscan_smoke(TEXT("ParticleSystem'/Game/ParticleSystems/PS_HitscanSmoke.PS_HitscanSmoke'"));
	hitscanDamageParticleSystems.Add(hitscan_smoke.Object);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> hitscanBurst_Ranged(TEXT("ParticleSystem'/Game/ParticleSystems/PS_HitscanBurst_Ranged.PS_HitscanBurst_Ranged'"));
	hitscanWeakpointDamageParticleSystems.Add(hitscanBurst_Ranged.Object);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> swordHit_Ranged(TEXT("ParticleSystem'/Game/ParticleSystems/PS_SwordHit_Ranged.PS_SwordHit_Ranged'"));
	swordDamageParticleSystems.Add(swordHit_Ranged.Object);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> swordWeakpointHit_Ranged(TEXT("ParticleSystem'/Game/ParticleSystems/PS_SwordWeakpointHit_Ranged.PS_SwordWeakpointHit_Ranged'"));
	swordWeakpointDamageParticleSystems.Add(swordWeakpointHit_Ranged.Object);
	//TODO: add minor PS with sparks....

}