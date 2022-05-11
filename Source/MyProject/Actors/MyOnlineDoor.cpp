// Fill out your copyright notice in the Description page of Project Settings.


#include "MyOnlineDoor.h"
#include "Components/StaticMeshComponent.h"

void AMyOnlineDoor::TimelineProgress(float Value)
{
	FVector NewLocation = FMath::Lerp(StartLoc, EndLoc, Value);
	Door->SetRelativeLocation(NewLocation);
}

// Sets default values
AMyOnlineDoor::AMyOnlineDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));
	RootComponent = DoorFrame;
	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(DoorFrame);
}

// Called when the game starts or when spawned
void AMyOnlineDoor::BeginPlay()
{
	Super::BeginPlay();
	if (CurveFloat)
	{
		FOnTimelineFloat Progress;
		Progress.BindUFunction(this, FName("TimelineProgress"));
		CurveTimeLine.AddInterpFloat(CurveFloat, Progress);
		CurveTimeLine.SetLooping(true);

		StartLoc = EndLoc = Door->GetRelativeLocation();
		EndLoc.Y = 145.f;
		CurveTimeLine.PlayFromStart();
	}
	
}

// Called every frame
void AMyOnlineDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CurveTimeLine.TickTimeline(DeltaTime);

}

