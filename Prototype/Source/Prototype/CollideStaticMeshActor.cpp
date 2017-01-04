// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "CollideStaticMeshActor.h"

#include "HealthComponent.h"

ACollideStaticMeshActor::ACollideStaticMeshActor()
{
	OnActorHit.AddDynamic(this, &ACollideStaticMeshActor::OnHit);

	USceneComponent* rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = rootComp;

}

void ACollideStaticMeshActor::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, TEXT("HIT"));

	//Do some calc for damage...
	float damage = 10.f;
	TSubclassOf<UDamageType> const gunDamageType = TSubclassOf<UDamageType>(UDamageType::StaticClass()); //TODO GunDamageType, SwordDamageType, ...

	UGameplayStatics::ApplyDamage(SelfActor, damage, nullptr, OtherActor, gunDamageType);
}