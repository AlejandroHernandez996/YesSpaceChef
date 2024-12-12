#include "LoginHandler.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineIdentityInterface.h"

void ULoginHandler::Login()
{
    IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
    if (Subsystem != nullptr)
    {
        IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
        if (Identity.IsValid())
        {
            ELoginStatus::Type LoginStatus = Identity->GetLoginStatus(0); // Check the login status

            // If the user is not logged in, then attempt to log in
            if (LoginStatus == ELoginStatus::NotLoggedIn)
            {
                LoginDelegateHandle = Identity->AddOnLoginCompleteDelegate_Handle(0, FOnLoginComplete::FDelegate::CreateUObject(this, &ULoginHandler::HandleLoginComplete));

                if (!Identity->AutoLogin(0))
                {
                    // Handle error when login process couldn't start
                }
            }
            else
            {
                // Handle case when the user is already logged in
                bIsLoggedIn = true;
            }
        }
    }
}

void ULoginHandler::HandleLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
    // Check bWasSuccessful to see if the login was completed successfully.

    IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
    if (Subsystem != nullptr)
    {
        IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
        if (Identity.IsValid())
        {
            Identity->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, LoginDelegateHandle);
            LoginDelegateHandle.Reset();
        }
    }

    bIsLoggedIn = bWasSuccessful;

}