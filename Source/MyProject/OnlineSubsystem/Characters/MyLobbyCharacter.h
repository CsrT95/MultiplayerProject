// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyLobbyCharacter.generated.h"

UCLASS()
class MYPROJECT_API AMyLobbyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMyLobbyCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;
	void StartMatch();

};
