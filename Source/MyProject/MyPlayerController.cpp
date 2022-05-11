#include "MyPlayerController.h"
#include "MyProjectGameMode.h"
#include "Kismet/GameplayStatics.h"

void AMyPlayerController::RespawnPawn_Implementation(const FTransform& SpawnTransform)
{
	AMyProjectGameMode* MyGameMode = Cast<AMyProjectGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (MyGameMode)
	{
		MyGameMode->RequestSpawn(this, SpawnTransform);
		InformRespawn();
	}
}

void AMyPlayerController::InformRespawn_Implementation()
{
	OnCharacterRespawned.Broadcast();
}
