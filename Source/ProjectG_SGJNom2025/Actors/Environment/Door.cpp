// Copyright Offmeta

#include "Door.h"

ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
		SetRootComponent(RootComponent);
	}

	DoorPivotPoint = CreateDefaultSubobject<USceneComponent>(TEXT("PivotComponent"));
	DoorPivotPoint->SetupAttachment(GetRootComponent());
	
	DoorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMeshComponent"));
	DoorMeshComponent->SetupAttachment(GetRootComponent());
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	// InitialDoorLocation = DoorMeshComponent->GetRelativeLocation();
	// InitialDoorRotation = DoorMeshComponent->GetRelativeRotation();
	// PivotLocation = DoorPivotPoint->GetRelativeLocation();
 //    
	// RotationAlpha = 0.0f;
	// bIsRotating = false;
	// TargetRotation = InitialDoorRotation;
	//
	// TotalRotationAngle = FRotator::MakeFromEuler(RotateTillAngles).GetNormalized().Vector().Size() * 90.0f;

	InitialDoorLocation = DoorMeshComponent->GetRelativeLocation();
	InitialDoorRotation = DoorMeshComponent->GetRelativeRotation();
	PivotLocation = DoorPivotPoint->GetRelativeLocation();
    
	// Calculate the final rotation (open state)
	TargetRotation = InitialDoorRotation + FRotator::MakeFromEuler(RotateTillAngles);
    
	RotationAlpha = 0.0f;
	bIsRotating = false;
    
	// Calculate the total rotation angle in degrees
	const FRotator DeltaRotation = TargetRotation - InitialDoorRotation;
	TotalRotationAngle = DeltaRotation.GetNormalized().Vector().Size() * 90.0f;
    
	// If TotalRotationAngle is 0, set the minimum value.
	if (TotalRotationAngle <= 0.0f)
	{
		TotalRotationAngle = 1.0f;
	}
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRotating)
	{
		RotateDoor(DeltaTime);
	}
}

void ADoor::RotateDoor(float DeltaTime)
{
	const float AlphaChange = DeltaTime * (RotationSpeed / TotalRotationAngle);
	
	if (bIsOpening)
	{
		RotationAlpha += AlphaChange;
	}
	else
	{
		RotationAlpha -= AlphaChange;
	}
    
	// Restrict rotation alpha to [0, 1].
	RotationAlpha = FMath::Clamp(RotationAlpha, 0.0f, 1.0f);
    
	// Do linear interpolation for rotation.
	const FRotator NewRotation = FMath::Lerp(InitialDoorRotation, TargetRotation, RotationAlpha);
    
	// Calculate the offset for rotation around the pivot point.
	const FVector Offset = InitialDoorLocation - PivotLocation;
    
	// Rotate the offset
	const FVector RotatedOffset = NewRotation.RotateVector(Offset);
    
	// Calculating a new position
	const FVector NewLocation = PivotLocation + RotatedOffset;
    
	// Apply the new position and rotation
	DoorMeshComponent->SetRelativeLocation(NewLocation);
	DoorMeshComponent->SetRelativeRotation(NewRotation);
    
	// Check if rotation is complete.
	if ((bIsOpening && RotationAlpha >= 1.0f) || (!bIsOpening && RotationAlpha <= 0.0f))
	{
		bIsRotating = false;
	}
}

bool ADoor::Activate_Implementation()
{
	if (!bIsRotating || (bIsOpening && RotationAlpha < 1.0f) || (!bIsOpening && RotationAlpha > 0.0f))
	{
		bIsOpening = true;
		bIsRotating = true;
		
		return true;
	}
	return false;
}

bool ADoor::Deactivate_Implementation()
{
	if (!bIsRotating || (bIsOpening && RotationAlpha < 1.0f) || (!bIsOpening && RotationAlpha > 0.0f))
	{
		bIsOpening = false;
		bIsRotating = true;
		
		return true;
	}
	return false;
}
