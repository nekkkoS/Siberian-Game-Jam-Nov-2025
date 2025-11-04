// Copyright Offmeta

#include "TriggerActivatable.h"

#include "Components/BoxComponent.h"
#include "../../Interfaces/ActivatableInterface.h"

ATriggerActivatable::ATriggerActivatable()
{
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
		SetRootComponent(RootComponent);
	}

	ActivatablesBoxTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	ActivatablesBoxTrigger->SetupAttachment(RootComponent);
}

void ATriggerActivatable::BeginPlay()
{
	Super::BeginPlay();

	if (ActivatablesBoxTrigger)
	{
		ActivatablesBoxTrigger->OnComponentBeginOverlap.AddDynamic(
			this, &ATriggerActivatable::OnActivatablesTriggerStartOverlap);
		ActivatablesBoxTrigger->OnComponentEndOverlap.AddDynamic(
			this, &ATriggerActivatable::OnActivatablesTriggerEndOverlap);
	}
}

void ATriggerActivatable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATriggerActivatable::ActivateActivactables()
{
	if (ActivatableRefs.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Deactivating Activatables"));
		for (AActor* ActivatableRef : ActivatableRefs)
		{
			if (ActivatableRef && ActivatableRef->GetClass()->ImplementsInterface(UActivatableInterface::StaticClass()))
			{
				if (const IActivatableInterface* Activatable = Cast<IActivatableInterface>(ActivatableRef))
				{
					Activatable->Execute_Activate(ActivatableRef);
				}
			}
		}
	}
}

void ATriggerActivatable::DeactivateActivatables()
{
	if (ActivatableRefs.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Deactivating Activatables"));
		for (AActor* ActivatableRef : ActivatableRefs)
		{
			if (ActivatableRef && ActivatableRef->GetClass()->ImplementsInterface(UActivatableInterface::StaticClass()))
			{
				if (const IActivatableInterface* Activatable = Cast<IActivatableInterface>(ActivatableRef))
				{
					Activatable->Execute_Deactivate(ActivatableRef);
				}
			}
		}
	}
}

void ATriggerActivatable::OnActivatablesTriggerStartOverlap(UPrimitiveComponent* OverlappedComponent,
                                                            AActor* OtherActor,
                                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                            bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bCanActivateActorsOnStartOverlap)
	{
		return;
	}
	
	// Only trigger once logic.
	if (bTriggerOnce && bHasStartOverlapTriggered)
	{
		return;
	}
	if (bTriggerOnce && !bHasStartOverlapTriggered)
	{
		bHasStartOverlapTriggered = true;
	}
	
	ActivateActivactables();
}

void ATriggerActivatable::OnActivatablesTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!bCanDeactivateActorsOnEndOverlap)
	{
		return;
	}

	// Only trigger once logic.
	if (bTriggerOnce && bHasEndOverlapTriggered)
	{
		return;
	}
	if (bTriggerOnce && !bHasEndOverlapTriggered)
	{
		bHasEndOverlapTriggered = true;
	}
	
	GetWorld()->GetTimerManager().SetTimer(
		TimerEndOverlapDelayHandle,
		this,
		&ATriggerActivatable::OnDelayCompleted,
		DeactivationDelay,
		false
	);
}

void ATriggerActivatable::OnDelayCompleted()
{
	DeactivateActivatables();
}
