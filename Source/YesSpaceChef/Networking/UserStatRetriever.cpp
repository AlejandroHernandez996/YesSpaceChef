// Fill out your copyright notice in the Description page of Project Settings.

#include "UserStatRetriever.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineStatsInterface.h"

void UUserStatRetriever::RetrieveStats(TArray<TSharedRef<const FUniqueNetId>> UserNetIds, TArray<FString> StatNames) {
    IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlineStatsPtr StatsInterface = Subsystem->GetStatsInterface();
    for (TSharedRef<const FUniqueNetId> UserNetId : UserNetIds) {
        FString Id = UserNetId.Get().ToString();
        if (!PlayerStats.Contains(Id)) {
            TMap<FString, int32> StatsMap;
            PlayerStats.Add(Id, StatsMap);
        }
    }
    // The first parameter is the user performing the querying.
    StatsInterface->QueryStats(
        Identity->GetUniquePlayerId(0).ToSharedRef(),
        UserNetIds,
        StatNames,
        FOnlineStatsQueryUsersStatsComplete::CreateLambda([this](
            const FOnlineError& ResultState,
            const TArray<TSharedRef<const FOnlineStatsUserStats>>& UsersStatsResult)
            {
                if (!ResultState.bSucceeded)
                {
                    return;
                }

                // Each entry in UsersStatsResult is for a user.
                for (auto QueriedStats : UsersStatsResult)
                {
                    // Each entry in Stats is for a stat for that user.
                    for (auto KV : QueriedStats->Stats)
                    {
                        // KV.Key is the stat name.
                        int32 Value;
                        KV.Value.GetValue(Value);
                        FString PlayerId = QueriedStats->Account.Get().ToString();
                        if (PlayerStats.Contains(PlayerId)) {
                            PlayerStats[PlayerId].Add(KV.Key, Value);
                        }
                    }
                }
                bRetrievedStats = true;
            }));
}

int32 UUserStatRetriever::GetControllerStatInt32(APlayerController* InController, FString StatName) {
    if (!bRetrievedStats) return MIN_int32;

    FString ControllerNetId = GetControllerUniqueNetId(InController).Get()->ToString();

    if (PlayerStats.Contains(ControllerNetId) && PlayerStats[ControllerNetId].Contains(StatName)){
        return PlayerStats[ControllerNetId][StatName];
    }

    return MIN_int32;

}

TSharedPtr<const FUniqueNetId> UUserStatRetriever::GetControllerUniqueNetId(APlayerController* InPlayerController)
{
    if (!IsValid(InPlayerController))
    {
        return nullptr;
    }

    if (InPlayerController->IsLocalPlayerController())
    {
        ULocalPlayer* LocalPlayer = InPlayerController->GetLocalPlayer();
        if (IsValid(LocalPlayer))
        {
            return LocalPlayer->GetPreferredUniqueNetId().GetUniqueNetId();
        }
    }

    UNetConnection* RemoteNetConnection = Cast<UNetConnection>(InPlayerController->Player);
    if (IsValid(RemoteNetConnection))
    {
        return RemoteNetConnection->PlayerId.GetUniqueNetId();
    }

    UE_LOG(LogTemp, Error, TEXT("Player controller does not have a valid remote network connection"));
    return nullptr;
}