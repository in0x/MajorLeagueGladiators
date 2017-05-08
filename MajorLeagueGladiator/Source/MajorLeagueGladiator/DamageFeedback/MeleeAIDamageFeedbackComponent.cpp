// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "MeleeAIDamageFeedbackComponent.h"


UMeleeAIDamageFeedbackComponent::UMeleeAIDamageFeedbackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> hitscan_smoke(TEXT("ParticleSystem'/Game/ParticleSystems/PS_HitscanSmoke.PS_HitscanSmoke'"));
	hitscanDamageParticleSystems.Add(hitscan_smoke.Object);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> hitscanBurst_Melee(TEXT("ParticleSystem'/Game/ParticleSystems/PS_HitscanBurst_Melee.PS_HitscanBurst_Melee'"));
	hitscanWeakpointDamageParticleSystems.Add(hitscanBurst_Melee.Object);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> swordHit_Melee(TEXT("ParticleSystem'/Game/ParticleSystems/PS_SwordHit_Melee.PS_SwordHit_Melee'"));
	swordDamageParticleSystems.Add(swordHit_Melee.Object);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> swordWeakpointHit_Melee(TEXT("ParticleSystem'/Game/ParticleSystems/PS_SwordWeakpointHit_Melee.PS_SwordWeakpointHit_Melee'"));
	swordWeakpointDamageParticleSystems.Add(swordWeakpointHit_Melee.Object);
	//TODO: add minor PS with sparks....
	
}

