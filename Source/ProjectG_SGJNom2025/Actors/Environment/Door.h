// Copyright Offmeta

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectG_SGJNom2025/Interfaces/ActivatableInterface.h"
#include "Door.generated.h"

UCLASS()
class PROJECTG_SGJNOM2025_API ADoor : public AActor, public IActivatableInterface
{
	GENERATED_BODY()

public:
	ADoor();
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Environment | Door")
	TObjectPtr<USceneComponent> DoorPivotPoint;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Environment | Door")
	TObjectPtr<UStaticMeshComponent> DoorMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Environment | Door")
	FVector RotateTillAngles = FVector(0.0f, 0.0f, 90.0f);
	FVector InitialRotationAngles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Environment | Door")
	float RotationSpeed = 45.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Environment | Door")
	bool bIsOpening = false;
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Door|Sound")
	USoundBase* DoorOpenSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Door|Sound")
	USoundBase* DoorCloseSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Door|Sound")
	USoundBase* DoorRotateSound;

private:
	FVector InitialDoorLocation;
	FRotator InitialDoorRotation;
	FVector PivotLocation;
    
	float RotationAlpha;
	bool bIsRotating;
	FRotator TargetRotation;
	
	float TotalRotationAngle;
	
	void RotateDoor(float DeltaTime);
	virtual bool Activate_Implementation() override;
	virtual bool Deactivate_Implementation() override;

	bool bRotateSoundPlaying = false;
};
