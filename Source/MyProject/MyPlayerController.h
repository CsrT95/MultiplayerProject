#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterRespawned);

UCLASS()
class MYPROJECT_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
		FOnCharacterRespawned OnCharacterRespawned;
	UFUNCTION(Server, Reliable)
		void RespawnPawn(const FTransform& SpawnTransform);
	UFUNCTION(NetMulticast, Reliable)
		void InformRespawn();

	
};
