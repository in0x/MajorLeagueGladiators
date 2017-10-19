// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FriendlistWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFriendsRefreshRequestDelegate);

UCLASS()
class MAJORLEAGUEGLADIATOR_API UFriendlistWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnFriendsRefreshRequestDelegate FriendsRefreshRequest;

private:
	UFUNCTION(BlueprintCallable)
	void EmitFriendsRefreshRequest();
};
