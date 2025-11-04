// Copyright Offmeta

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "DefaultSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FPlayerSaveData
{
	GENERATED_BODY()

	FPlayerSaveData()
		: PlayerTransform(FTransform::Identity)
		, PlayerRotation(FRotator::ZeroRotator)
	{}

	UPROPERTY(BlueprintReadWrite, Category = "Save")
	FTransform PlayerTransform;

	UPROPERTY(BlueprintReadWrite, Category = "Save")
	FRotator PlayerRotation;
};

/**
 * 
 */
UCLASS()
class PROJECTG_SGJNOM2025_API UDefaultSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Save")
	FPlayerSaveData PlayerData;
};
