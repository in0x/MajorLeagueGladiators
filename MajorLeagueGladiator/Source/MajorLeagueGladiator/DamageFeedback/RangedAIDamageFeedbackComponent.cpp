// Fill out your copyright notice in the Description page of Project Settings.

#include "MajorLeagueGladiator.h"
#include "RangedAIDamageFeedbackComponent.h"


URangedAIDamageFeedbackComponent::URangedAIDamageFeedbackComponent()
{
	static ConstructorHelpers::FObjectFinder<UParticleSystem> hitscan_smoke(TEXT("ParticleSystem'/Game/ParticleSystems/PS_HitscanSmoke.PS_HitscanSmoke'"));
	hitscanDamageParticleSystems.Add(hitscan_smoke.Object);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> hitscanBurst_Melee(TEXT("ParticleSystem'/Game/ParticleSystems/PS_HitscanBurst_Melee.PS_HitscanBurst_Melee'"));
	hitscanWeakpointDamageParticleSystems.Add(hitscanBurst_Melee.Object);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> swordHit_Melee(TEXT("ParticleSystem'/Game/ParticleSystems/PS_SwordHit_Melee.PS_SwordHit_Melee'"));
	swordDamageParticleSystems.Add(swordHit_Melee.Object);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> swordWeakpointHit_Melee(TEXT("ParticleSystem'/Game/ParticleSystems/PS_SwordWeakpointHit_Melee.PS_SwordWeakpointHit_Melee'"));
	swordWeakpointDamageParticleSystems.Add(swordWeakpointHit_Melee.Object);
	
	/*
	Original ranged ones: orange
	static ConstructorHelpers::FObjectFinder<UParticleSystem> hitscan_smoke(TEXT("ParticleSystem'/Game/ParticleSystems/PS_HitscanSmoke.PS_HitscanSmoke'"));
	hitscanDamageParticleSystems.Add(hitscan_smoke.Object);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> hitscanBurst_Ranged(TEXT("ParticleSystem'/Game/ParticleSystems/PS_HitscanBurst_Ranged.PS_HitscanBurst_Ranged'"));
	hitscanWeakpointDamageParticleSystems.Add(hitscanBurst_Ranged.Object);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> swordHit_Ranged(TEXT("ParticleSystem'/Game/ParticleSystems/PS_SwordHit_Ranged.PS_SwordHit_Ranged'"));
	swordDamageParticleSystems.Add(swordHit_Ranged.Object);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> swordWeakpointHit_Ranged(TEXT("ParticleSystem'/Game/ParticleSystems/PS_SwordWeakpointHit_Ranged.PS_SwordWeakpointHit_Ranged'"));
	swordWeakpointDamageParticleSystems.Add(swordWeakpointHit_Ranged.Object);
	*/
	//TODO: add minor PS with sparks....

}