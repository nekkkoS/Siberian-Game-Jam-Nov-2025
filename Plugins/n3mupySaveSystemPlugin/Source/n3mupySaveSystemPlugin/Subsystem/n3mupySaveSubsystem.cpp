// Copyright n3mupy

#include "n3mupySaveSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY(SaveLog);

void Un3mupySaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	GetWorld()->GetTimerManager().SetTimerForNextTick([&]()
	{
		AsyncLoadDelegate.BindUObject(this, &Un3mupySaveSubsystem::HandleAsyncLoad);
		AsyncSaveDelegate.BindUObject(this, &Un3mupySaveSubsystem::HandleAsyncSave);
	
		LoadGameData();
	});
}

void Un3mupySaveSubsystem::Deinitialize()
{
	OnGameLoadedDelegate.Clear();
	OnGameAsyncLoadedDelegate.Clear();
	OnGameSavedDelegate.Clear();
	OnGameAsyncSavedDelegate.Clear();
	OnGameLoadedBlueprintDelegate.Clear();
	OnGameAsyncLoadedBlueprintDelegate.Clear();
	OnGameSavedBlueprintDelegate.Clear();
	OnGameAsyncSavedBlueprintDelegate.Clear();
	
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
			bIsSaveLoaded = true;
			
			OnGameLoadedDelegate.Broadcast(SavedData);
			OnGameLoadedBlueprintDelegate.Broadcast(SavedData);
			UE_LOG(SaveLog, Display, TEXT("Game loaded in SaveSystem"));
		}
	}
	else
	{
		SavedData = UGameplayStatics::CreateSaveGameObject(USaveGame::StaticClass());
		bIsSaveLoaded = true;
		
		OnGameLoadedDelegate.Broadcast(SavedData);
		OnGameLoadedBlueprintDelegate.Broadcast(SavedData);
		UE_LOG(SaveLog, Display, TEXT("Game loaded in SaveSystem"));
	}
}

void Un3mupySaveSubsystem::SaveGameData(bool bIsAsync) const
{
	if (!SavedData)
	{
		return;
	}

	if (bIsAsync)
	{
		UGameplayStatics::AsyncSaveGameToSlot(SavedData, SaveSlotName, SaveUserIndex, AsyncSaveDelegate);
	}
	else
	{
		const bool bIsSaved = UGameplayStatics::SaveGameToSlot(SavedData, SaveSlotName, SaveUserIndex);
		
		OnGameSavedDelegate.Broadcast(SavedData, bIsSaved);
		OnGameSavedBlueprintDelegate.Broadcast(SavedData, bIsSaved);
		UE_LOG(SaveLog, Display, TEXT("Data saved by SaveSystem"));
	}
}

bool Un3mupySaveSubsystem::IsSaveGameExists(const FString& _SlotName, int32 _UserIndex)
{
	if (_SlotName.IsEmpty())
	{
		return false;
	}
	
	return UGameplayStatics::DoesSaveGameExist(_SlotName, _UserIndex);
}

bool Un3mupySaveSubsystem::DeleteSavedGame(const FString& _SlotName, const int32 _UserIndex)
{
	if (IsSaveGameExists(_SlotName, _UserIndex))
	{
		UE_LOG(SaveLog, Display, TEXT("Save deleted by DeleteSavedGame in SaveSystem"));
		return UGameplayStatics::DeleteGameInSlot(_SlotName, _UserIndex);
	}
	return false;
}

void Un3mupySaveSubsystem::BindToOnGameLoadedDelegate(UObject* Listener, const FName& FunctionName)
{
	if (!Listener) return;

	// Assign BP event to delegate
	FScriptDelegate D;
	D.BindUFunction(Listener, FunctionName);
	OnGameLoadedBlueprintDelegate.Add(D);
	OnGameAsyncLoadedBlueprintDelegate.Add(D);

	// If the save is already loaded - call ONLY the JUST added listener directly,
	// without global Broadcast (so as not to double calls for the others)
	if (bIsSaveLoaded && SavedData)
	{
		if (UFunction* Func = Listener->FindFunction(FunctionName))
		{
			struct
			{
				USaveGame* SaveType;
			} Params{SavedData};

			Listener->ProcessEvent(Func, &Params);
		}
	}
}

void Un3mupySaveSubsystem::HandleAsyncLoad(const FString& SlotName, int32 UserIndex, USaveGame* LoadedData)
{
	bIsSaveLoaded = true;
	OnGameAsyncLoadedDelegate.Broadcast(LoadedData);
	OnGameAsyncLoadedBlueprintDelegate.Broadcast(LoadedData);
	UE_LOG(SaveLog, Display, TEXT("Game async loaded by SaveSystem"));
}

void Un3mupySaveSubsystem::HandleAsyncSave(const FString& InSlot, int32 InUserIdx, bool bSuccess)
{
	OnGameAsyncSavedDelegate.Broadcast(SavedData, bSuccess);
	OnGameAsyncSavedBlueprintDelegate.Broadcast(SavedData, bSuccess);
	UE_LOG(SaveLog, Display, TEXT("Game async saved by SaveSystem"));
}
