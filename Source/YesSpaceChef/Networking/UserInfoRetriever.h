// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UserInfoRetriever.generated.h"

/**
 * 
 */
UCLASS()
class YESSPACECHEF_API UUserInfoRetriever : public UObject
{
	GENERATED_BODY()
	
public:
	void GetAllUserNetIdsToDisplayNames(TArray<TSharedRef<const FUniqueNetId>> UniqueNetIds);
	TSharedPtr<const FUniqueNetId> GetControllerUniqueNetId(APlayerController* InPlayerController);

	bool bRetrievedAllUsers;

	void SetUserNetIdsToDisplayName(APlayerController* InPlayerController);

	TMap<FString, FString> UserDisplayNameMap;
protected:
	FDelegateHandle QueryUserInfoDelegateHandle;
	void HandleQueryUserInfoComplete(
		int32 LocalUserNum,
		bool bWasSuccessful,
		const TArray<TSharedRef<const FUniqueNetId>>& UserIds,
		const FString& ErrorStr);
};
