// Copyright Offmeta

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TriggerActivatable.generated.h"

UCLASS()
class PROJECTG_SGJNOM2025_API ATriggerActivatable : public AActor
{
	GENERATED_BODY()

public:
	ATriggerActivatable();
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Environment | Activatable")
	TArray<AActor*> ActivatableRefs;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Environment | Activatable")
	bool bTriggerOnce = true;
	bool bHasStartOverlapTriggered = false;
	bool bHasEndOverlapTriggered = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Environment | Activatable")
	bool bCanActivateActorsOnStartOverlap = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Environment | Activatable")
	bool bCanDeactivateActorsOnEndOverlap = false;

	virtual void BeginPlay() override;

private:
	FTimerHandle TimerEndOverlapDelayHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Environment | Activatable",
		meta = (AllowPrivateAccess = "true"))
	float DeactivationDelay = 4.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PuzzleManager", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBoxComponent> ActivatablesBoxTrigger;

	void ActivateActivactables();
	void DeactivateActivatables();

	UFUNCTION()
	void OnActivatablesTriggerStartOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                       const FHitResult& SweepResult);

	UFUNCTION()
	void OnActivatablesTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnDelayCompleted();
};
