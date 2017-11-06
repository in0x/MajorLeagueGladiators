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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInviteFriendRequestDelegate, int32, FriendIndex);

USTRUCT(BlueprintType)
struct FFriendState
{
	GENERATED_BODY()

	FFriendState() = default;

	FFriendState(const FString& Username, UTexture2D* AvatarTex, int32 FriendIndex, EOnlineState::Type OnlineState, bool bJoinable, bool bInvitable)
		: username(Username)
		, avatar(AvatarTex)
		, friendIndex(FriendIndex)
		, onlineState(OnlineState)
		, bCanJoin(bJoinable)
		, bCanInvite(bInvitable)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString username;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* avatar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 friendIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EOnlineState::Type> onlineState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanJoin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanInvite;
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

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ChangeCanInvite(bool bInvitable);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ChangeAvatar(UTexture2D* texture);

	UFUNCTION(BlueprintCallable)
	void ChangeFriendIndex(int32 Index);

	UPROPERTY(BlueprintAssignable)
	FJoinFriendRequestDelegate JoinFriendRequested;

	UPROPERTY(BlueprintAssignable)
	FInviteFriendRequestDelegate InviteFriendRequested;

	UFUNCTION(BlueprintCallable)
	const FString& GetUsername() const;

	UFUNCTION(BlueprintCallable)
	int32 GetFriendIndex() const;

	UFUNCTION(BlueprintCallable)
	bool GetCanJoin() const;

	UFUNCTION(BlueprintCallable)
	bool GetCanInvite() const;

protected:
	UFUNCTION(BlueprintCallable)
	void EmitJoinFriendRequested() const;

	UFUNCTION(BlueprintCallable)
	void EmitInviteFriendRequested() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString username;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 friendIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EOnlineState::Type> onlineState;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanJoin;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanInvite;
};
