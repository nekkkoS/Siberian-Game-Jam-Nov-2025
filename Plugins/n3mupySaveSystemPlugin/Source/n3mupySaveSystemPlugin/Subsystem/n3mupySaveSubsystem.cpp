// Copyright n3mupy

#include "n3mupySaveSubsystem.h"

#include "TimerManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "../DeveloperSettings/n3mupySaveSettings.h"
#include "GameFramework/SaveGame.h"

DEFINE_LOG_CATEGORY(SaveLog);

void Un3mupySaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const Un3mupySaveSettings* SaveSettings = Un3mupySaveSettings::Get();

	// Settings should exist.
	checkf(SaveSettings,
	       TEXT(
		       "N3mupy Save System settings not found! Please configure in Project Settings -> Plugins -> N3mupy Save System"
	       ));

	// Class should be set in settings.
	checkf(SaveSettings->DefaultSaveGameClass,
	       TEXT(
		       "DefaultSaveGameClass is not set in N3mupy Save System settings! Please set a valid SaveGame class in Project Settings -> Plugins -> N3mupy Save System"
	       ));

	// Class should not be base USaveGame (abstract).	
	checkf(SaveSettings->DefaultSaveGameClass != USaveGame::StaticClass(),
	       TEXT(
		       "DefaultSaveGameClass must be a custom class, not the base USaveGame! Please set a custom SaveGame class in Project Settings -> Plugins -> N3mupy Save System"
	       ));

	SaveGameClass = SaveSettings->DefaultSaveGameClass;
	SaveSlotName = SaveSettings->DefaultSlotName;
	SaveUserIndex = SaveSettings->DefaultUserIndex;
	
	// Check if the SaveGameClass is valid
	checkf(SaveGameClass && SaveGameClass != USaveGame::StaticClass(),
	       TEXT("SaveGameClass is invalid! It must be a custom class derived from USaveGame"));

	AsyncLoadDelegate.BindUObject(this, &Un3mupySaveSubsystem::HandleAsyncLoad);
	AsyncSaveDelegate.BindUObject(this, &Un3mupySaveSubsystem::HandleAsyncSave);
	
	GetWorld()->GetTimerManager().SetTimerForNextTick([&]()
	{
		LoadGameData();
	});
}

void Un3mupySaveSubsystem::Deinitialize()
{
	OnGameLoadedDelegate.Clear();
	OnGameSavedDelegate.Clear();
	OnSaveSystemReadyDelegate.Clear();

	Super::Deinitialize();
}

void Un3mupySaveSubsystem::LoadGameData(bool bIsAsync)
{
	bIsSaveLoaded = false;
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex))
	{
		if (bIsAsync)
		{
			UGameplayStatics::AsyncLoadGameFromSlot(SaveSlotName, SaveUserIndex, AsyncLoadDelegate);
		}
		else
		{
			SavedData = UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserIndex);
			
			// Проверяем тип загруженных данных
			if (SavedData && SavedData->IsA(SaveGameClass))
			{
				bIsSaveLoaded = true;
                
				UE_LOG(LogTemp, Warning, TEXT("Game data loaded successfully"));
				OnGameLoadedDelegate.Broadcast(SavedData);
				OnGameLoadedBlueprintDelegate.Broadcast(SavedData);
				MarkSubsystemReady();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Loaded data has wrong type, creating new"));
				SavedData = UGameplayStatics::CreateSaveGameObject(SaveGameClass);
				bIsSaveLoaded = true;
                
				OnGameLoadedDelegate.Broadcast(SavedData);
				OnGameLoadedBlueprintDelegate.Broadcast(SavedData);
				MarkSubsystemReady();
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Save doesn't exist, creating new"));
		SavedData = UGameplayStatics::CreateSaveGameObject(SaveGameClass);
		bIsSaveLoaded = true;
        
		UE_LOG(LogTemp, Warning, TEXT("New save data created"));
		OnGameLoadedDelegate.Broadcast(SavedData);
		OnGameLoadedBlueprintDelegate.Broadcast(SavedData);
		MarkSubsystemReady();
	}
}

void Un3mupySaveSubsystem::SaveGameData(bool bIsAsync)
{
	if (!SavedData)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot save - no data available"));
		return;
	}

	if (bIsAsync)
	{
		UGameplayStatics::AsyncSaveGameToSlot(SavedData, SaveSlotName, SaveUserIndex, AsyncSaveDelegate);
	}
	else
	{
		bool bSuccess = UGameplayStatics::SaveGameToSlot(SavedData, SaveSlotName, SaveUserIndex);
        
		UE_LOG(LogTemp, Warning, TEXT("Game data saved: %s"), bSuccess ? TEXT("Success") : TEXT("Failed"));
		OnGameSavedDelegate.Broadcast(SavedData, bSuccess);
		OnGameSavedBlueprintDelegate.Broadcast(SavedData, bSuccess);
	}
}

bool Un3mupySaveSubsystem::IsSaveGameExists(const FString& SlotName, int32 UserIndex)
{
	const FString ActualSlotName = SlotName.IsEmpty() ? SaveSlotName : SlotName;
	const int32 ActualUserIndex = UserIndex == -1 ? SaveUserIndex : UserIndex;
    
	return UGameplayStatics::DoesSaveGameExist(ActualSlotName, ActualUserIndex);
}

bool Un3mupySaveSubsystem::DeleteSavedGame(const FString& SlotName, const int32 UserIndex)
{
	const FString ActualSlotName = SlotName.IsEmpty() ? SaveSlotName : SlotName;
	const int32 ActualUserIndex = UserIndex == -1 ? SaveUserIndex : UserIndex;
    
	if (UGameplayStatics::DoesSaveGameExist(ActualSlotName, ActualUserIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("Deleting save game: %s"), *ActualSlotName);
		return UGameplayStatics::DeleteGameInSlot(ActualSlotName, ActualUserIndex);
	}
    
	return false;
}

void Un3mupySaveSubsystem::SubscribeToSaveSystemBlueprint(UObject* Listener, 
														 FName OnReadyFunction, 
														 FName OnLoadedFunction, 
														 FName OnSavedFunction)
{
	if (!Listener) return;

	UE_LOG(LogTemp, Warning, TEXT("SubscribeToSaveSystemBlueprint: Setting up %s"), *Listener->GetName());

	// Подписываем на готовность системы
	if (!OnReadyFunction.IsNone())
	{
		FScriptDelegate ReadyDelegate;
		ReadyDelegate.BindUFunction(Listener, OnReadyFunction);
		OnSaveSystemReadyBlueprintDelegate.Add(ReadyDelegate);
	}

	// Подписываем на загрузку данных
	if (!OnLoadedFunction.IsNone())
	{
		FScriptDelegate LoadedDelegate;
		LoadedDelegate.BindUFunction(Listener, OnLoadedFunction);
		OnGameLoadedBlueprintDelegate.Add(LoadedDelegate);
	}

	// Подписываем на сохранение данных
	if (!OnSavedFunction.IsNone())
	{
		FScriptDelegate SavedDelegate;
		SavedDelegate.BindUFunction(Listener, OnSavedFunction);
		OnGameSavedBlueprintDelegate.Add(SavedDelegate);
	}

	// Если система уже готова - немедленно вызываем колбек готовности
	if (bIsSubsystemReady && !OnReadyFunction.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("System already ready, calling OnReady immediately"));
		UFunction* Function = Listener->FindFunction(OnReadyFunction);
		if (Function)
		{
			Listener->ProcessEvent(Function, nullptr);
		}
	}

	// Если данные уже загружены - немедленно вызываем колбек загрузки
	if (bIsSaveLoaded && SavedData && !OnLoadedFunction.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("Data already loaded, calling OnLoaded immediately"));
		UFunction* Function = Listener->FindFunction(OnLoadedFunction);
		if (Function)
		{
			struct FOnGameLoadedParams
			{
				USaveGame* SaveType;
			} Params;
			Params.SaveType = SavedData;
            
			Listener->ProcessEvent(Function, &Params);
		}
	}
}

void Un3mupySaveSubsystem::MarkSubsystemReady()
{
	if (!bIsSubsystemReady)
	{
		bIsSubsystemReady = true;
		UE_LOG(LogTemp, Warning, TEXT("=== Save System READY ==="));
		OnSaveSystemReadyDelegate.Broadcast();
		OnSaveSystemReadyBlueprintDelegate.Broadcast();
	}
}

void Un3mupySaveSubsystem::HandleAsyncLoad(const FString& SlotName, int32 UserIndex, USaveGame* LoadedData)
{
	if (LoadedData && LoadedData->IsA(SaveGameClass))
	{
		SavedData = LoadedData;
		bIsSaveLoaded = true;
        
		UE_LOG(LogTemp, Warning, TEXT("Game data loaded successfully (async)"));
		OnGameLoadedDelegate.Broadcast(SavedData);
		OnGameLoadedBlueprintDelegate.Broadcast(SavedData);
		MarkSubsystemReady();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Async loaded data has wrong type, creating new"));
		SavedData = UGameplayStatics::CreateSaveGameObject(SaveGameClass);
		bIsSaveLoaded = true;
        
		OnGameLoadedDelegate.Broadcast(SavedData);
		OnGameLoadedBlueprintDelegate.Broadcast(SavedData);
		MarkSubsystemReady();
	}

	UE_LOG(LogTemp, Warning, TEXT("Async load completed"));
}

void Un3mupySaveSubsystem::HandleAsyncSave(const FString& InSlot, int32 InUserIdx, bool bSuccess)
{
	OnGameSavedDelegate.Broadcast(SavedData, bSuccess);
	OnGameSavedBlueprintDelegate.Broadcast(SavedData, bSuccess);
	
	UE_LOG(LogTemp, Warning, TEXT("Async save completed: %s"), bSuccess ? TEXT("Success") : TEXT("Failed"));
}
