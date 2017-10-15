// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FriendWidget.h"
#include "FriendsMenuActor.generated.h"

class FOnlineFriend;
class UWidgetComponent;

UCLASS()
class MAJORLEAGUEGLADIATOR_API AFriendsMenuActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AFriendsMenuActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnFriendsInfoLoaded(const TArray<FFriendState>& friends);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetComponent* widgetComponent;

private:
	void OnFriendlistLoaded(const TArray<TSharedRef<FOnlineFriend>>& friendlist);
};
