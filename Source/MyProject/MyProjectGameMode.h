// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyProjectGameMode.generated.h"

class APlayerController;

UCLASS(minimalapi)
class AMyProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyProjectGameMode();

	UFUNCTION(Server, Reliable)
		void RequestSpawn(APlayerController* PlayerController, const FTransform& SpawnTransform);
};