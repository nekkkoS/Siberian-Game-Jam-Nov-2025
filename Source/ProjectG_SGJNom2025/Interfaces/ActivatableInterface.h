// Copyright Offmeta

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ActivatableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UActivatableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTG_SGJNOM2025_API IActivatableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	bool Activate();

	UFUNCTION(BlueprintNativeEvent)
	bool Deactivate();
};
