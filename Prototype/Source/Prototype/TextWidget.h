// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "TextWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE_API UTextWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="TextWidget")
	void SetText(const FString& Text)
	{
		text = Text;
	}
	
	UFUNCTION(BlueprintCallable, Category = "TextWidget")
	FString GetText() const
	{
		return text;
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TextWidget")
	FString text;
};
