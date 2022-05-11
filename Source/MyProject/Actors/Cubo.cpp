#include "Cubo.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACubo::ACubo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));

	StaticMeshComponent->SetupAttachment(RootComponent);
	BoxComponent->SetupAttachment(StaticMeshComponent);
}

// Called when the game starts or when spawned
void ACubo::BeginPlay()
{
	Super::BeginPlay();

	MyMaterial = StaticMeshComponent->CreateDynamicMaterialInstance(0, MySourceMaterial);

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ACubo::BeginOverlap);
}

void ACubo::Multi_ChangeColor_Implementation(const FLinearColor& NewColor)
{
	CurrentColor = NewColor;
	if (MyMaterial)
	{
		MyMaterial->SetVectorParameterValue("Color", CurrentColor);
	}
}

// Called every frame
void ACubo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACubo::GenerateRandomColor()
{
	CurrentColor = UKismetMathLibrary::MakeColor(UKismetMathLibrary::RandomFloatInRange(0.f, 1.f), UKismetMathLibrary::RandomFloatInRange(0.f, 1.f), UKismetMathLibrary::RandomFloatInRange(0.f, 1.f));
}

void ACubo::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		GenerateRandomColor();
		Multi_ChangeColor(CurrentColor);
	}
}

