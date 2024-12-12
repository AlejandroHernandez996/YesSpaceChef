// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h" 
#include "SessionHandler.generated.h"

/**
 * 
 */
UCLASS()
class YESSPACECHEF_API USessionHandler : public UObject
{
	GENERATED_BODY()

public:
	void CreateSession();

	UFUNCTION(BlueprintCallable, Category = "Session")
		void FindSession();
	UFUNCTION(BlueprintCallable, Category = "Session")
		void JoinSession();
	UFUNCTION(BlueprintCallable, Category = "Session")
		void DestroySession();

	FString GetConnectionInfo() { return ConnectionInfo; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
		bool bHasFoundSession;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
		bool bHasJoinedSession;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
		bool bHasCreatedSession;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
		bool bFailedSession;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
		bool bCancelFindSessionFlag;


private:
	FDelegateHandle CreateSessionDelegateHandle;
	FDelegateHandle FindSessionsDelegateHandle;
	FDelegateHandle JoinSessionDelegateHandle;
	FDelegateHandle DestroySessionDelegateHandle;

	FOnlineSessionSearchResult SearchResult;
	FString ConnectionInfo;

	void HandleCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void HandleFindSessionsComplete(bool bWasSuccessful, TSharedRef<FOnlineSessionSearch> Search);
	void HandleJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult);
	void HandleDestroySessionComplete(FName SessionName, bool bWasSuccessful);
};
