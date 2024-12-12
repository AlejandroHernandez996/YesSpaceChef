// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UserStatRetriever.generated.h"

/**
 * 
 */
UCLASS()
class YESSPACECHEF_API UUserStatRetriever : public UObject
{
	GENERATED_BODY()
	
public:
	void RetrieveStats(TArray<TSharedRef<const FUniqueNetId>> UserNetIds, TArray<FString> StatNames);

	UFUNCTION(BlueprintCallable, Category = "Retrieve Stats")
		int32 GetControllerStatInt32(APlayerController* InController, FString StatName);

	TMap<FString, TMap<FString, int32>> PlayerStats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Retrieve Stats")
		bool bRetrievedStats;
	
	TSharedPtr<const FUniqueNetId> GetControllerUniqueNetId(APlayerController* InPlayerController);
};