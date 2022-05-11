#include "MyLobbyCharacter.h"
#include "OnlineSubsystem/GameInstance/MyOnlineGameInstance.h"
#include "Kismet/GameplayStatics.h"

AMyLobbyCharacter::AMyLobbyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMyLobbyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMyLobbyCharacter::StartMatch()
{
	if (UMyOnlineGameInstance* GameInstance = Cast<UMyOnlineGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		GameInstance->StartSession();
	}
}

void AMyLobbyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (HasAuthority())
	{
		PlayerInputComponent->BindAction("StartMatch", EInputEvent::IE_Pressed, this, &AMyLobbyCharacter::StartMatch);
	}
}

