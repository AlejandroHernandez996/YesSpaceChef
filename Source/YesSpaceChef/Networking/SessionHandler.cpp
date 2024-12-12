#include "SessionHandler.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "YesSpaceChef/YesSpaceChefGameMode.h"

void USessionHandler::CreateSession()
{
    IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
    IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

    this->CreateSessionDelegateHandle =
        Session->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionComplete::FDelegate::CreateUObject(
            this,
            &USessionHandler::HandleCreateSessionComplete));

    TSharedRef<FOnlineSessionSettings> SessionSettings = MakeShared<FOnlineSessionSettings>();
    SessionSettings->NumPublicConnections = 2; // The number of players.
    SessionSettings->bShouldAdvertise = true;  // Set to true to make this session discoverable with FindSessions.
    SessionSettings->bUsesPresence = false;    // Set to true if you want this session to be discoverable by presence (Epic Social Overlay).

    // You *must* set at least one setting value, because you can not run FindSessions without any filters.
    SessionSettings->Settings.Add(
        FName(TEXT("SessionSetting")),
        FOnlineSessionSetting(FString(TEXT("SettingValue")), EOnlineDataAdvertisementType::ViaOnlineService));

    // Create a session and give the local name "MyLocalSessionName". This name is entirely local to the current player and isn't stored in EOS.
    if (!Session->CreateSession(0, FName(TEXT("MyLocalSessionName")), *SessionSettings))
    {
        // Call didn't start, return error.
        bHasCreatedSession = false;
    }
}

void USessionHandler::HandleCreateSessionComplete(FName SessionName,bool bWasSuccessful)
{
    // TODO: Check bWasSuccessful to see if the session was created.

    // Deregister the event handler.
    IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
    IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
    Session->ClearOnCreateSessionCompleteDelegate_Handle(this->CreateSessionDelegateHandle);
    this->CreateSessionDelegateHandle.Reset();

    AYesSpaceChefGameMode* MyGameMode = Cast<AYesSpaceChefGameMode>(GetWorld()->GetAuthGameMode());
    MyGameMode->RegisterExistingPlayers();
    bHasCreatedSession = bWasSuccessful;
}

void USessionHandler::FindSession()
{
    IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
    IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

    TSharedRef<FOnlineSessionSearch> Search = MakeShared<FOnlineSessionSearch>();
    // Remove the default search parameters that FOnlineSessionSearch sets up.
    Search->QuerySettings.SearchParams.Empty();

    // Add your search settings here. If you're including both listening and non-listening sessions as per the __EOS_bListening example above, then you must include at least one additional filter, or EOS will not return any search results.
    Search->QuerySettings.Set(
        FName(TEXT("SessionSetting")),
        FString(TEXT("SettingValue")),
        EOnlineComparisonOp::Equals);

    this->FindSessionsDelegateHandle =
        Session->AddOnFindSessionsCompleteDelegate_Handle(FOnFindSessionsComplete::FDelegate::CreateUObject(
            this,
            &USessionHandler::HandleFindSessionsComplete,
            Search));

    if (!Session->FindSessions(0, Search))
    {
        // Call didn't start, return error.
        bFailedSession = true;
    }
}

void USessionHandler::HandleFindSessionsComplete(
    bool bWasSuccessful,
    TSharedRef<FOnlineSessionSearch> Search)
{
    IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
    IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
    if (bCancelFindSessionFlag) {
        bFailedSession = true;
        bCancelFindSessionFlag = false;
        bHasFoundSession = false;
        return;
    }
    
    if (bWasSuccessful)
    {
        for (auto RawResult : Search->SearchResults)
        {
            if (RawResult.IsValid())
            {
                FString ConnectInfo;
                if (Session->GetResolvedConnectString(RawResult, NAME_GamePort, ConnectInfo))
                {
                    // Store both RawResult and ConnectInfo in an array or some kind of temporary storage. You will need both values when you join the session later.

                    // If you only want to get the first result, break the loop here.
                    SearchResult = RawResult;
                    ConnectionInfo = ConnectInfo;
                    break;
                }
            }
        }
    }

    Session->ClearOnFindSessionsCompleteDelegate_Handle(this->FindSessionsDelegateHandle);
    this->FindSessionsDelegateHandle.Reset();
    bHasFoundSession = bWasSuccessful;
}

void USessionHandler::JoinSession()
{
    IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
    IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

    this->JoinSessionDelegateHandle =
        Session->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionComplete::FDelegate::CreateUObject(
            this,
            &USessionHandler::HandleJoinSessionComplete));

    // "MyLocalSessionName" is the local name of the session for this player. It doesn't have to match the name the server gave their session.
    if (!Session->JoinSession(0, FName(TEXT("ClientSession")), SearchResult))
    {
        bFailedSession = true;
    }
}

void USessionHandler::HandleJoinSessionComplete(
    FName SessionName,
    EOnJoinSessionCompleteResult::Type JoinResult)
{
    if (JoinResult == EOnJoinSessionCompleteResult::Success ||
        JoinResult == EOnJoinSessionCompleteResult::AlreadyInSession)
    {
        // Use the connection string that you got from FindSessions in order
        // to connect to the server.
        //
        // Refer to "Connecting to a game server" under the "Networking & Anti-Cheat"
        // section of the documentation for more information on how to do this.
        //
        // NOTE: You can also call GetResolvedConnectString at this point instead
        // of in FindSessions, but it's recommended that you call it in
        // FindSessions so you know the result is valid.
        bHasJoinedSession = true;
    }
    else {
        bFailedSession = true;
    }

    IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
    IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

    Session->ClearOnJoinSessionCompleteDelegate_Handle(this->JoinSessionDelegateHandle);
    this->JoinSessionDelegateHandle.Reset();

}

void USessionHandler::DestroySession() {
    IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
    IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

    this->DestroySessionDelegateHandle =
        Session->AddOnDestroySessionCompleteDelegate_Handle(FOnDestroySessionComplete::FDelegate::CreateUObject(
            this,
            &USessionHandler::HandleDestroySessionComplete));

    // "MyLocalSessionName" is the local name of the session for the client or server. It should be the value you specified when creating or joining the session (depending on which you called).
    if (!Session->DestroySession(FName(TEXT("MyLocalSessionName"))))
    {
        // Call didn't start, return error.
    }
    else {
        //FGenericPlatformMisc::RequestExit(false);
    }
}

void USessionHandler::HandleDestroySessionComplete(
    FName SessionName,
    bool bWasSuccessful)
{
    // TODO: Check bWasSuccessful to see if the session was created.

    // Deregister the event handler.
    IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
    IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
    Session->ClearOnDestroySessionCompleteDelegate_Handle(this->DestroySessionDelegateHandle);
    this->DestroySessionDelegateHandle.Reset();
}