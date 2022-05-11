// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "MyOnlineDoor.generated.h"

class UStaticMeshComponent;

UCLASS()
class MYPROJECT_API AMyOnlineDoor : public AActor
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
		UStaticMeshComponent* DoorFrame;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
		UStaticMeshComponent* Door;
	UPROPERTY(EditDefaultsOnly, Category = "Timeline")
		UCurveFloat* CurveFloat;

	UFUNCTION()
	void TimelineProgress(float Value);

	AMyOnlineDoor();
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY()
		FVector StartLoc;
	UPROPERTY()
		FVector EndLoc;

	virtual void BeginPlay() override;
	FTimeline CurveTimeLine;

private:
	bool bCurveIsActive;



};
