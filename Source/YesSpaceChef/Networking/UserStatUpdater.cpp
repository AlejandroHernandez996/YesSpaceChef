// Fill out your copyright notice in the Description page of Project Settings.


#include "UserStatUpdater.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineStatsInterface.h"

void UUserStatUpdater::UpdateInt32Stat(TSharedRef<const FUniqueNetId> UserNetId, FString StatName, int32 IngestAmount) {
    IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlineStatsPtr StatsInterface = Subsystem->GetStatsInterface();

    FOnlineStatsUserUpdatedStats Stat = FOnlineStatsUserUpdatedStats(UserNetId);

    // For each of the stats to upload, provide the StatName (matching what
    // is defined in the Epic Online Services portal) and the int32 value.
    // The EOnlineStatModificationType is ignored, as the stat type is defined
    // in the portal.
    //
    // You can add multiple entries to the Stats property to send more than one
    // stat value in a single request.
    Stat.Stats.Add(StatName, FOnlineStatUpdate(IngestAmount, FOnlineStatUpdate::EOnlineStatModificationType::Unknown));

    TArray<FOnlineStatsUserUpdatedStats> Stats;
    Stats.Add(Stat);

    StatsInterface->UpdateStats(
        UserNetId,
        Stats,
        FOnlineStatsUpdateStatsComplete::CreateLambda([this](
            const FOnlineError& ResultState)
            {
                if (ResultState.bSucceeded) {
                    bSucceeded = ResultState.bSucceeded;
                }
            }));
}