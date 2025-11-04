// Copyright Offmeta

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameEndProviderInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UGameEndProviderInterface : public UInterface
{
	GENERATED_BODY()
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameEndSignature, bool /*bIsGameEnded*/);

/**
 * 
 */
class PROJECTG_SGJNOM2025_API IGameEndProviderInterface
{
	GENERATED_BODY()

public:
	virtual FOnGameEndSignature& SubscribeToOnGameEndDelegate() = 0;
};
