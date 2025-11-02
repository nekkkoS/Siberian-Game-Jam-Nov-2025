// Copyright n3mupy

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "n3mupySaveSubsystem.generated.h"

/**
*	To use this plugin, user should create their own USaveGame derived class,
*	then subscribe to OnGameLoadedDelegate / OnGameSavedDelegate (C++ or BP versions)
*	and cast USaveGame* parameter to their own class in callback function.
*
*	Example (without casting to own USaveGame class):
*	APlayerCharacter.cpp
*	void APlayerFirstPerson::PossessedBy(AController* NewController)
*	{
*		Super::PossessedBy(NewController);
*
*		if (Un3mupySaveSubsystem* SaveSubsystem = GetGameInstance()->GetSubsystem<Un3mupySaveSubsystem>())
*		{
*			SaveSubsystem->SubscribeToOnGameLoadedDelegate(this, &APlayerFirstPerson::UseSavedData);
*			SaveSubsystem->OnGameSavedDelegate.AddUObject(this, &APlayerFirstPerson::AddDataForSave);
*		}
*	}
*	void APlayerFirstPerson::UseSavedData(ULSSaveGame* SavedData)
*	{
*		if (!SavedData.PlayerSave.PlayerTransform.Equals(FTransform::Identity))
*		{
*			SetActorTransform(SavedData.PlayerSave.PlayerTransform);
*			GetController()->SetControlRotation(SavedData.PlayerSave.PlayerRotation);
*			SanityComponent->SetSanity(SavedData.PlayerSave.Sanity);
*			InventoryComponent->Slots = SavedData.PlayerSave.SavedInventory;
*		}
*	}
*
*	void APlayerFirstPerson::AddDataForSave(ULSSaveGame& SavedData)
*	{
*		SavedData.PlayerSave = FPlayerSave(
*				GetActorTransform(),
*				GetControlRotation(),
*				SanityComponent->GetSanity(),
*				InventoryComponent->Slots
*			);
*	}
*
*	To save game somewhere in code use:
*	void UPauseMenuWidget::SaveGame()
*	{
*		if (Un3mupySaveSubsystem* SaveSubsystem = GetGameInstance()->GetSubsystem<Un3mupySaveSubsystem>())
*		{
*			SaveSubsystem->SaveGameData();
*
*			if (SaveSubsystem->IsSaveGameExists())
*			{
*				LoadGameButton->SetIsEnabled(true);
*			}
*		}
*	}
*/

DECLARE_LOG_CATEGORY_EXTERN(SaveLog, Display, All);

// C++ delegates
	// Sync
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameLoadedSignature, USaveGame* /** Save type */)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGameSavedSignature, USaveGame* /** Save type */, bool /* bSuccess */)
	// Async
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameAsyncLoadedSignature, USaveGame* /** Save type */);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGameAsyncSavedSignature, USaveGame* /** Save type */, bool /* bSuccess */);

// BP delegates
	// Sync
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameLoadedBPSignature, USaveGame*, SaveType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameSavedBPSignature, USaveGame*, SaveType, bool, bSuccess);
	// Async
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameAsyncLoadedBPSignature, USaveGame*, SaveType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGameAsyncSavedBPSignature, USaveGame*, SaveType, bool, bSuccess);

/**
 * Author: n3mupy\n
 * Subsystem for saving game data with usage of USaveGame. Designed for simple singleplayer games.\n
 * User should create their own USaveGame and cast to it in delegate callback functions.
 */
UCLASS(meta = (ToolTip = "Subsystem for saving game data with usage of USaveGame. Designed for simple singleplayer games.\n User should create their own USaveGame and cast to it in delegate callback functions."))
class N3MUPYSAVESYSTEMPLUGIN_API Un3mupySaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** Delegate for saving data to USaveGame. */
	FOnGameSavedSignature OnGameSavedDelegate;
	FOnGameAsyncSavedSignature OnGameAsyncSavedDelegate;

	// Async C++ delegates for callbacks
	FAsyncLoadGameFromSlotDelegate AsyncLoadDelegate;
	FAsyncSaveGameToSlotDelegate AsyncSaveDelegate;
	
	UFUNCTION(BlueprintCallable, Category = "n3mupy | SaveSystem")
	virtual void LoadGameData(bool bIsAsync = false);

	UFUNCTION(BlueprintCallable, Category = "n3mupy | SaveSystem")
	virtual void SaveGameData(bool bIsAsync = false) const;

	UFUNCTION(BlueprintCallable, Category = "n3mupy | SaveSystem")
	virtual bool IsSaveGameExists(const FString& _SlotName, const int32 _UserIndex);

	UFUNCTION(BlueprintCallable, Category = "n3mupy | SaveSystem")
	virtual bool DeleteSavedGame(const FString& _SlotName, const int32 _UserIndex);

	/** Deferred subscription for FOnGameLoadedSignature (C++ version). */
	template <typename UserClass>
	void SubscribeToOnGameLoadedDelegate(UserClass* Object, void (UserClass::*Func)(USaveGame*))
	{
		OnGameLoadedDelegate.AddUObject(Object, Func);
		OnGameAsyncLoadedDelegate.AddUObject(Object, Func);

		// If the save is already loaded - call ONLY the JUST added listener directly,
		// without global Broadcast (so as not to double calls for the others)
		if (bIsSaveLoaded && SavedData)
		{
			(Object->*Func)(SavedData);
		}
	}

	/** Deferred subscription for OnGameLoadedBlueprintDelegate (BP version). */
	UFUNCTION(BlueprintCallable, Category="n3mupy | SaveSystem", meta = (ToolTip = "Deferred subscription for OnGameLoadedBlueprintDelegate"))
	void BindToOnGameLoadedDelegate(UObject* Listener, const FName& FunctionName);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "n3mupy | SaveSystem")
	TObjectPtr<USaveGame> SavedData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "n3mupy | SaveSystem")
	FString SaveSlotName = "Save1";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "n3mupy | SaveSystem")
	int32 SaveUserIndex = 0;
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:

	/** Delegate for notify subscribers about USaveGame is available for use.
	 * For example loading player position at start of the game.
	 */
	FOnGameLoadedSignature OnGameLoadedDelegate;
	FOnGameAsyncLoadedSignature OnGameAsyncLoadedDelegate;

	/** Delegate for notify subscribers about USaveGame is available for use.
	 * For example loading player position at start of the game.
	 */
	//UPROPERTY(BlueprintAssignable, Category="n3mupy | SaveSystem")
	FOnGameLoadedBPSignature OnGameLoadedBlueprintDelegate;
	FGameAsyncLoadedBPSignature OnGameAsyncLoadedBlueprintDelegate;

	/** Delegate for saving data to USaveGame. */
	UPROPERTY(BlueprintAssignable, Category="n3mupy | SaveSystem", meta = (ToolTip = "Delegate for saving data to USaveGame."))
	FOnGameSavedBPSignature OnGameSavedBlueprintDelegate;

	UPROPERTY(BlueprintAssignable, Category="n3mupy | SaveSystem", meta = (ToolTip = "Delegate for saving data to USaveGame."))
	FGameAsyncSavedBPSignature OnGameAsyncSavedBlueprintDelegate;
	
	bool bIsSaveLoaded = false;

	UFUNCTION()
	void HandleAsyncLoad(const FString& SlotName, int32 UserIndex, USaveGame* LoadedData);

	UFUNCTION()
	void HandleAsyncSave(const FString& InSlot, int32 InUserIdx, bool bSuccess);
	
public:
	const USaveGame* GetGameData() const { return SavedData; }
};
