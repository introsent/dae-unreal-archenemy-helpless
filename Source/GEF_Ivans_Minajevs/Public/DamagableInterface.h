// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamagableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDamagableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GEF_IVANS_MINAJEVS_API IDamagableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "DoDamage Reaction")
	void DoDamage(int amount, bool& isDead);
};
