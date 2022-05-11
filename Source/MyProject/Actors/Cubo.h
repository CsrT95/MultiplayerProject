#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cubo.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class MYPROJECT_API ACubo : public AActor
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(EditDefaultsOnly, Category = "Cubo")
		UStaticMeshComponent* StaticMeshComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Cubo")
		UBoxComponent* BoxComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cubo")
		UMaterialInterface* MySourceMaterial;
	UPROPERTY()
		FLinearColor CurrentColor;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	ACubo();
	virtual void Tick(float DeltaTime) override;
	void GenerateRandomColor();
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
		UMaterialInstanceDynamic* MyMaterial;

	UFUNCTION(NetMulticast, Reliable)
		void Multi_ChangeColor(const FLinearColor& NewColor);
	

};
