// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FriendWidget.h"
#include "Online.h"
#include "FriendsMenuActor.generated.h"

class FOnlineFriend;
class UWidgetComponent;

UCLASS(Blueprintable)
class MAJORLEAGUEGLADIATOR_API AFriendsMenuActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AFriendsMenuActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	
protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnFriendsInfoLoaded(const TArray<FFriendState>& friends);

	void OnFriendsInfoLoaded_Implementation(const TArray<FFriendState>& friends);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetComponent* widgetComponent;

	UFUNCTION()
	void OnJoinFriendRequest(int32 friendIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UFriendWidget*> friendWidgets;

private:
	void OnFriendlistLoaded(const TArray<TSharedRef<FOnlineFriend>>& friendlist);

	UFUNCTION()
	void OnFriendsRefreshRequested();

	UFUNCTION(BlueprintCallable)
	void BindToFriendWidget(UFriendWidget* Widget);

	UFUNCTION(BlueprintCallable)
	void HideUnusedWidgets(int32 LastUsedIndex);

	void FindFriendSession(TSharedRef<FOnlineFriend> Friend);
	void OnFindFriendSessionComplete(int32 LocalUserNum, bool bWasSuccessful, const TArray<FOnlineSessionSearchResult>& SearchResult);

	FOnFindFriendSessionCompleteDelegate onFindFriendSessionCompleteDelegate;
	FDelegateHandle onFindFriendSessionCompleteDelegateHandle;

	void UpdateAvatarTextureCount(int32 CurrentNumFriends);

	UPROPERTY()
	TArray<UTexture2D*> avatarTextures;
};
