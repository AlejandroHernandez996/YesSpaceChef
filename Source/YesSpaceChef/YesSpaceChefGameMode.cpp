// Copyright Epic Games, Inc. All Rights Reserved.

#include "YesSpaceChefGameMode.h"

#include "YesSpaceChefCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Networking/SessionHandler.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

AYesSpaceChefGameMode::AYesSpaceChefGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AYesSpaceChefGameMode::RegisterExistingPlayers()
{
	for (auto It = this->GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PlayerController = It->Get();

		FUniqueNetIdRepl UniqueNetIdRepl;
		if (PlayerController->IsLocalPlayerController())
		{
			ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
			if (IsValid(LocalPlayer))
			{
				UniqueNetIdRepl = LocalPlayer->GetPreferredUniqueNetId();
			}
			else
			{
				UNetConnection* RemoteNetConnection = Cast<UNetConnection>(PlayerController->Player);
				check(IsValid(RemoteNetConnection));
				UniqueNetIdRepl = RemoteNetConnection->PlayerId;
			}
		}
		else
		{
			UNetConnection* RemoteNetConnection = Cast<UNetConnection>(PlayerController->Player);
			check(IsValid(RemoteNetConnection));
			UniqueNetIdRepl = RemoteNetConnection->PlayerId;
		}

		// Get the unique player ID.
		TSharedPtr<const FUniqueNetId> UniqueNetId = UniqueNetIdRepl.GetUniqueNetId();
		check(UniqueNetId != nullptr);

		// Get the online session interface.
		IOnlineSubsystem* Subsystem = Online::GetSubsystem(PlayerController->GetWorld());
		IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

		// Register the player with the "MyLocalSessionName" session; this name should match the name you provided in CreateSession.
		if (!Session->RegisterPlayer(FName(TEXT("MyLocalSessionName")), *UniqueNetId, false))
		{
			// The player could not be registered; typically you will want to kick the player from the server in this situation.
		}
	}
}