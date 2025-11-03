// Copyright Offmeta

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PuzzleChipProviderInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UPuzzleChipProviderInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTG_SGJNOM2025_API IPuzzleChipProviderInterface
{
	GENERATED_BODY()

public:
	virtual void ShowRandomPuzzleChip() = 0;
};
