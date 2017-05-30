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
		StartRangedTutorial UMETA(DisplayName = "StartRangedTutorial") 
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

private:
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EMenuAction::Type> menuAction;
};
