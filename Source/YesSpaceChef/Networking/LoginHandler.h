#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LoginHandler.generated.h"

UCLASS()
class YESSPACECHEF_API ULoginHandler : public UObject
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "Login")
        void Login();
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login")
        bool bIsLoggedIn;
private:
    FDelegateHandle LoginDelegateHandle;

    void HandleLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
};