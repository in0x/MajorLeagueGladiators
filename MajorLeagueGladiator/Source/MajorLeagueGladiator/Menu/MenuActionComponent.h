// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "MenuAction.h"
#include "MenuActionComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAJORLEAGUEGLADIATOR_API UMenuActionComponent : public UActorComponent
{
	GENERATED_BODY()	
public:	
	UMenuActionComponent();
	void TriggerMenuAction() const;
	FMenuActionDelegate OnMenuActionTriggered;
	void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EMenuAction::Type> menuAction;

	UFUNCTION()
	void onActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void onActorOverlap(AActor* OverlappedActor, AActor* OtherActor);
};
