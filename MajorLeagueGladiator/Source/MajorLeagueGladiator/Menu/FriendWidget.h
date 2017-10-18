// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FriendWidget.generated.h"

UENUM(BlueprintType)
namespace EOnlineState
{
	enum Type
	{
		InGame  UMETA(DisplayName = "InGame"),
		Online	 UMETA(DisplayName = "Online"),
		Offline  UMETA(DisplayName = "Offline")
	};
}

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FJoinFriendRequestDelegate, int32, FriendIndex);

USTRUCT(BlueprintType)
struct FFriendState
{
	GENERATED_BODY()

	FFriendState() = default;

	FFriendState(const FString& Username, EOnlineState::Type OnlineState, bool bJoinable)
		: username(Username)
		, onlineState(OnlineState)
		, bCanJoin(bJoinable)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString username;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EOnlineState::Type> onlineState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanJoin;
};

UCLASS(Blueprintable)
class MAJORLEAGUEGLADIATOR_API UFriendWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ChangeOnlineState(EOnlineState::Type OnlineState);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ChangeUsername(const FString& Name);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ChangeCanJoin(bool bJoinable);

	UFUNCTION(BlueprintCallable)
	void ChangeFriendIndex(int32 Index);

	UPROPERTY(BlueprintAssignable)
	FJoinFriendRequestDelegate JoinFriendRequested;

protected:
	UFUNCTION(BlueprintCallable)
	void EmitJoinFriendRequested() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString username;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 friendIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EOnlineState::Type> onlineState;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanJoin;
	
	bool bCanInvite;
};
