#include "OnlineSubsystem/GameInstance/MyOnlineGameInstance.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UMyOnlineGameInstance::StartSession()
{
	GetWorld()->ServerTravel("/Game/ThirdPersonCPP/Maps/ThirdPersonExampleMap?listen");
}

void UMyOnlineGameInstance::HostGameSession()
{
	if (SessionInterface)
	{
		FOnlineSessionSettings SessionSettings;

		SessionSettings.bIsDedicated = false;
		SessionSettings.bIsLANMatch = true;
		SessionSettings.bUsesPresence = true;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.NumPublicConnections = 4;

		SessionInterface->CreateSession(0, "Mi sesion", SessionSettings);
	}
}

void UMyOnlineGameInstance::FindGameSessions()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());

	SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UMyOnlineGameInstance::JoinGameSession(int32 Index)
{
	if (SearchResults.Num() > 0 && Index < SearchResults.Num())
	{
		SessionInterface->JoinSession(0, *SearchResults[Index].GetSessionIdStr(), SearchResults[Index]);
	}
}

FBlueprintSessionResult UMyOnlineGameInstance::GetBPSessionResult(int32 Index) const
{
	FBlueprintSessionResult SessionResult;
	if (Index >= SearchResults.Num() || SearchResults.Num() == 0)
		return SessionResult;

	SessionResult.OnlineResult = SearchResults[Index];
	return SessionResult;
}

TArray<FBlueprintSessionResult> UMyOnlineGameInstance::GetBPSessionsResult() const
{
	TArray<FBlueprintSessionResult> Results;
	for (FOnlineSessionSearchResult SearchResult : SearchResults)
	{
		FBlueprintSessionResult Result;
		Result.OnlineResult = SearchResult;
		Results.Add(Result);
	}
	return Results;
}

void UMyOnlineGameInstance::Init()
{
	Super::Init();

	if (IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, 
				&UMyOnlineGameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, 
				&UMyOnlineGameInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, 
				&UMyOnlineGameInstance::OnJoinSessionComplete);
		}
	}
}

void UMyOnlineGameInstance::OnCreateSessionComplete(FName SessionName, bool bSucceded)
{
	if (bSucceded)
	{
		UE_LOG(LogTemp, Error, TEXT("Session Created succesfuly"));
		GetWorld()->ServerTravel("/Game/OnlineSubsystem/Maps/Lobby/LV_Lobby?listen");
	}
}

void UMyOnlineGameInstance::OnFindSessionsComplete(bool bSucceded)
{
	if (bSucceded)
	{
		SearchResults = SessionSearch->SearchResults;
	}
	else
	{
		SearchResults.Empty();
	}

	OnFindGameSessionsComplete.Broadcast();
}

void UMyOnlineGameInstance::OnJoinSessionComplete(FName GameSession, EOnJoinSessionCompleteResult::Type Result)
{
	switch (Result)
	{
		case EOnJoinSessionCompleteResult::Type::Success:
		{
			if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
			{
				FString MapUrl;
				if (SessionInterface->GetResolvedConnectString(GameSession, MapUrl))
				{
					PlayerController->ClientTravel(MapUrl, ETravelType::TRAVEL_Absolute);
				}
			}
		}
		break;
	}
}
