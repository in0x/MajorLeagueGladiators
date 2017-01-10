// Fill out your copyright notice in the Description page of Project Settings.

#include "Prototype.h"
#include "ConstraintColliderComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

UConstraintColliderComponent::UConstraintColliderComponent()
{
	/*auto comp = GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass());

	if (comp)
	{
	skeleton = CastChecked<USkeletalMeshComponent>(comp);
	}*/

}
void UConstraintColliderComponent::BeginPlay()
{
	//Super::BeginPlay();
	OnComponentBeginOverlap.AddDynamic(this, &UConstraintColliderComponent::OnBeginOverlap);
	if(GetOwner())
		GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UConstraintColliderComponent::OnDamageTaken);
}



void UConstraintColliderComponent::OnBeginOverlap(UPrimitiveComponent* hitComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& hit)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Emerald, otherActor->GetName());
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Emerald, FString::Printf(TEXT("HIT by %s"), *otherActor->GetName()));

	auto comp = GetOwner()->GetComponentByClass(UPhysicsConstraintComponent::StaticClass());
	if (comp)
	{
		UPhysicsConstraintComponent* constraint = CastChecked<UPhysicsConstraintComponent>(comp);
		FVector linearForce;
		FVector angularForce;
		constraint->GetConstraintForce(linearForce, angularForce);
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Emerald, (linearForce.ToString()));
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Emerald, FString::Printf(TEXT("Force: %f"),linearForce.Size()));
		UE_LOG(LogTemp, Warning, TEXT("Force: %f"), linearForce.Size());
		if (linearForce.Size() > 5000000)
		{
			UGameplayStatics::ApplyDamage(GetOwner(), 100, nullptr, GetOwner(), TSubclassOf<UDamageType>(UDamageType::StaticClass()));
		}
	}
	//constraint->
}

void UConstraintColliderComponent::OnDamageTaken(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("DAMAGEEEE!"));
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("DAMAGEEEE!"));
	auto comp = GetOwner()->GetComponentByClass(UPhysicsConstraintComponent::StaticClass());
	if (comp)
	{
		UPhysicsConstraintComponent* constraint = CastChecked<UPhysicsConstraintComponent>(comp);
		constraint->BreakConstraint();
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Emerald, TEXT("constraint broken"));
	}
}


