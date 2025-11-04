// Copyright Offmeta

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameEndManager.generated.h"

UCLASS()
class PROJECTG_SGJNOM2025_API AGameEndManager : public AActor
{
	GENERATED_BODY()

public:
	AGameEndManager();
	virtual void Tick(float DeltaTime) override;

protected:
	TScriptInterface<class IGameEndProviderInterface> GameEndProvider;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Game End Manager")
	TObjectPtr<AActor> GameEndPuzzleManager;
	
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game End Manager")
	TObjectPtr<class UBoxComponent> GameEndTrigger;

	UFUNCTION()
	void OnGameEndTriggerStartOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
										UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
										const FHitResult& SweepResult);

	UFUNCTION()
	void OnGameEndTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
									  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
private:
	UPROPERTY(VisibleInstanceOnly, Category = "Game End Manager")
	bool bIsGameEnded = false;
	
	UFUNCTION()
	void OnGameEnd(bool bIsEnded);

	
};
