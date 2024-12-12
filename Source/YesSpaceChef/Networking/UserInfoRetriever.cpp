// Fill out your copyright notice in the Description page of Project Settings.
#include "UserInfoRetriever.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineUserInterface.h"

void UUserInfoRetriever::GetAllUserNetIdsToDisplayNames(TArray<TSharedRef<const FUniqueNetId>> UniqueNetIds) {
    IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
    IOnlineUserPtr User = Subsystem->GetUserInterface();

    this->QueryUserInfoDelegateHandle = User->AddOnQueryUserInfoCompleteDelegate_Handle(
        0, // Should match the LocalUserNum passed to `QueryUserInfo` below.
        FOnQueryUserInfoComplete::FDelegate::CreateUObject(
            this,
            &UUserInfoRetriever::HandleQueryUserInfoComplete));

    User->QueryUserInfo(0 /* LocalUserNum */, UniqueNetIds);
}

void UUserInfoRetriever::SetUserNetIdsToDisplayName(APlayerController* InPlayerController) {

    IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
    IOnlineUserPtr User = Subsystem->GetUserInterface();

    this->QueryUserInfoDelegateHandle = User->AddOnQueryUserInfoCompleteDelegate_Handle(
        0, // Should match the LocalUserNum passed to `QueryUserInfo` below.
        FOnQueryUserInfoComplete::FDelegate::CreateUObject(
            this,
            &UUserInfoRetriever::HandleQueryUserInfoComplete));
    TArray<TSharedRef<const FUniqueNetId>> UserIds;
    TSharedRef<const FUniqueNetId> Id = GetControllerUniqueNetId(InPlayerController).ToSharedRef();
    UserIds.Add(Id);

    User->QueryUserInfo(0 /* LocalUserNum */, UserIds);
}

void UUserInfoRetriever::HandleQueryUserInfoComplete(
    int32 LocalUserNum,
    bool bWasSuccessful,
    const TArray<TSharedRef<const FUniqueNetId>>& UserIds,
    const FString& ErrorStr)
{
    bRetrievedAllUsers = bWasSuccessful;
    // TODO: Check bWasSuccessful, if it wasn't successful the error will be in ErrorStr.

    IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
    IOnlineUserPtr User = Subsystem->GetUserInterface();

    if (bWasSuccessful)
    {
        for (TSharedRef<const FUniqueNetId> UserId : UserIds)
        {
            // You can now call GetUserInfo to get the account for the returned IDs.
            TSharedPtr<FOnlineUser> UserAcc = User->GetUserInfo(LocalUserNum, *UserId);
            UserDisplayNameMap.Add(UserId.Get().ToString(), UserAcc.Get()->GetDisplayName());
        }
    }

    User->ClearOnQueryUserInfoCompleteDelegate_Handle(LocalUserNum, this->QueryUserInfoDelegateHandle);
    this->QueryUserInfoDelegateHandle.Reset();
}

TSharedPtr<const FUniqueNetId> UUserInfoRetriever::GetControllerUniqueNetId(APlayerController* InPlayerController)
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