// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "MenuActionComponent.generated.h"

UENUM(BlueprintType)
namespace EMenuAction
{
	enum Type
	{
		HostGame			UMETA(DisplayName = "HostGame"),
		JoinGame			UMETA(DisplayName = "JoinGame"),
		StartMeleeTutorial	UMETA(DisplayName = "StartMeleeTutorial"),
		StartRangedTutorial UMETA(DisplayName = "StartRangedTutorial"),
		StartGameEasy		UMETA(DisplayName = "StartGameEasy"),
		StartGameMedium		UMETA(DisplayName = "StartGameMedium"),
		StartGameHard		UMETA(DisplayName = "StartGameHard")
	};
}

DECLARE_MULTICAST_DELEGATE_OneParam(FMenuActionDelegate, TEnumAsByte<EMenuAction::Type>);

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
