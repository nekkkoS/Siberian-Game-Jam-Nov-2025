// Copyright n3mupy

#pragma once

#include "CoreMinimal.h"
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

// Is subsystem ready delegates
DECLARE_MULTICAST_DELEGATE(FOnSaveSystemReadySignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSaveSubsystemReadyBlueprintSignature);

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
UCLASS(
	meta = (ToolTip =
		"Subsystem for saving game data with usage of USaveGame. Designed for simple singleplayer games.\n User should create their own USaveGame and cast to it in delegate callback functions."
	))
class N3MUPYSAVESYSTEMPLUGIN_API Un3mupySaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Data Load Delegate
    FOnGameLoadedSignature OnGameLoadedDelegate;
    UPROPERTY(BlueprintAssignable, Category = "n3mupy | SaveSystem")
    FOnGameLoadedBPSignature OnGameLoadedBlueprintDelegate;

    // Data persistence delegate  
    FOnGameSavedSignature OnGameSavedDelegate;
    UPROPERTY(BlueprintAssignable, Category = "n3mupy | SaveSystem")
    FOnGameSavedBPSignature OnGameSavedBlueprintDelegate;

    // Subsystem Readiness Delegate
    FOnSaveSystemReadySignature OnSaveSystemReadyDelegate;
    UPROPERTY(BlueprintAssignable, Category = "n3mupy | SaveSystem")
    FOnSaveSubsystemReadyBlueprintSignature OnSaveSystemReadyBlueprintDelegate;

    // ===== BASIC METHODS =====
    
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category = "n3mupy | SaveSystem")
    void LoadGameData(bool bIsAsync = false);

    UFUNCTION(BlueprintCallable, Category = "n3mupy | SaveSystem")
    void SaveGameData(bool bIsAsync = false);

    UFUNCTION(BlueprintCallable, Category = "n3mupy | SaveSystem")
    bool IsSaveGameExists(const FString& SlotName = "", int32 UserIndex = -1);

    UFUNCTION(BlueprintCallable, Category = "n3mupy | SaveSystem")
    bool DeleteSavedGame(const FString& SlotName = "", int32 UserIndex = -1);

    // ===== SIMPLIFIED SUBSCRIPTION METHODS =====

    /** The basic subscription method is where the user subscribes only to the availability and then consumes the data */
    template <typename UserClass>
    void SubscribeToSaveSystem(UserClass* Object, 
                               void (UserClass::*OnReady)(),
                               void (UserClass::*OnLoaded)(USaveGame*),
                               void (UserClass::*OnSaved)(USaveGame*, bool))
    {
        if (!Object) return;

        UE_LOG(LogTemp, Warning, TEXT("SubscribeToSaveSystem: Setting up %s"), *Object->GetName());

        // We sign off on the system's readiness
        OnSaveSystemReadyDelegate.AddUObject(Object, OnReady);

        // We are signing up for the main delegates
        OnGameLoadedDelegate.AddUObject(Object, OnLoaded);
        OnGameSavedDelegate.AddUObject(Object, OnSaved);

        // If the system is already ready, we immediately call the ready callback.
        if (bIsSubsystemReady)
        {
            UE_LOG(LogTemp, Warning, TEXT("System already ready, calling OnReady immediately"));
            (Object->*OnReady)();
        }

        // If the data has already been loaded, we immediately call the loading callback.
        if (bIsSaveLoaded && SavedData)
        {
            UE_LOG(LogTemp, Warning, TEXT("Data already loaded, calling OnLoaded immediately"));
            (Object->*OnLoaded)(SavedData);
        }
    }

    /** Blueprint version of the simplified subscription */
    UFUNCTION(BlueprintCallable, Category = "n3mupy | SaveSystem")
    void SubscribeToSaveSystemBlueprint(UObject* Listener, 
                                       FName OnReadyFunction, 
                                       FName OnLoadedFunction, 
                                       FName OnSavedFunction);

    // ===== GETTERS =====

    UFUNCTION(BlueprintCallable, Category = "n3mupy | SaveSystem")
    USaveGame* GetSaveData() const { return SavedData; }

    UFUNCTION(BlueprintCallable, Category = "n3mupy | SaveSystem")
    FString GetSaveSlotName() const { return SaveSlotName; }

    UFUNCTION(BlueprintCallable, Category = "n3mupy | SaveSystem")
    void SetSaveSlotName(const FString& NewSlotName) { SaveSlotName = NewSlotName; }

    UFUNCTION(BlueprintCallable, Category = "n3mupy | SaveSystem")
    int32 GetSaveUserIndex() const { return SaveUserIndex; }

    UFUNCTION(BlueprintCallable, Category = "n3mupy | SaveSystem")
    void SetSaveUserIndex(int32 NewUserIndex) { SaveUserIndex = NewUserIndex; }

    UFUNCTION(BlueprintCallable, Category = "n3mupy | SaveSystem")
    bool IsSubsystemReady() const { return bIsSubsystemReady; }

private:
    // ===== INTERNAL DATA =====
    
    UPROPERTY()
    TSubclassOf<USaveGame> SaveGameClass;

    UPROPERTY()
    FString SaveSlotName;

    UPROPERTY()
    int32 SaveUserIndex;

    UPROPERTY()
    USaveGame* SavedData = nullptr;

    bool bIsSaveLoaded = false;
    bool bIsSubsystemReady = false;

    // ===== INTERNAL DATA =====
    
    void MarkSubsystemReady();
    void HandleAsyncLoad(const FString& SlotName, int32 UserIndex, USaveGame* LoadedData);
    void HandleAsyncSave(const FString& SlotName, int32 UserIndex, bool bSuccess);

	// Async delegates for callbacks
	FAsyncLoadGameFromSlotDelegate AsyncLoadDelegate;
	FAsyncSaveGameToSlotDelegate AsyncSaveDelegate;
};
