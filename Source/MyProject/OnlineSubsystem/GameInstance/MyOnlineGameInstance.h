#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "FindSessionsCallbackProxy.h"
#include "MyOnlineGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFindGameSessionsComplete);

UCLASS()
class MYPROJECT_API UMyOnlineGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	void StartSession();

protected:
	UPROPERTY(BlueprintAssignable)
		FOnFindGameSessionsComplete OnFindGameSessionsComplete;
	UFUNCTION(BlueprintCallable)
		void HostGameSession();
	UFUNCTION(BlueprintCallable)
		void FindGameSessions();
	UFUNCTION(BlueprintCallable)
		void JoinGameSession(int32 Index);
	UFUNCTION(BlueprintCallable)
		FBlueprintSessionResult GetBPSessionResult(int32 Index) const;
	UFUNCTION(BlueprintCallable)
		TArray<FBlueprintSessionResult> GetBPSessionsResult() const;

	virtual void Init() override;
	void OnCreateSessionComplete(FName SessionName, bool bSucceded);
	void OnFindSessionsComplete(bool bSucceded);
	void OnJoinSessionComplete(FName GameSession, EOnJoinSessionCompleteResult::Type Result);
private:
	TArray<FOnlineSessionSearchResult> SearchResults;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	IOnlineSessionPtr SessionInterface;
	
};
