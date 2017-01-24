// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>
#include "UObject/NoExportTypes.h"

#include "Singleton.generated.h"

class UEventBus;
/**
 * 
 */
UCLASS()
class PROTOTYPE_API USingleton : public UObject
{
	GENERATED_BODY()
public:
	USingleton(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static USingleton& Get();
};
