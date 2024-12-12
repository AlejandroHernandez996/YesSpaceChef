// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UserStatUpdater.generated.h"

/**
 * 
 */
UCLASS()
class YESSPACECHEF_API UUserStatUpdater : public UObject
{
	GENERATED_BODY()
public:
	void UpdateInt32Stat(TSharedRef<const FUniqueNetId> UserNetId, FString StatName, int32 IngestAmount);
private:
	bool bSucceeded;
};