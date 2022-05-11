// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyProjectGameMode.h"
#include "MyProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

AMyProjectGameMode::AMyProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AMyProjectGameMode::RequestSpawn_Implementation(APlayerController* PlayerController, const FTransform& SpawnTransform)
{
	if (!PlayerController)
		return;

	if (HasAuthority())
	{
		AMyProjectCharacter* MyCharacter = GetWorld()->SpawnActor<AMyProjectCharacter>(DefaultPawnClass, SpawnTransform);
		if (MyCharacter)
		{
			UKismetSystemLibrary::Delay(GetWorld(), 0.2f, FLatentActionInfo());
			PlayerController->Possess(MyCharacter);
		}
	}
}
