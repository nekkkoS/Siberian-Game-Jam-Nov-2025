// Copyright Offmeta

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BlinkingProviderInterface.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnBlinkingEndedSignature)

// This class does not need to be modified.
UINTERFACE()
class UBlinkingProviderInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTG_SGJNOM2025_API IBlinkingProviderInterface
{
	GENERATED_BODY()

public:
	virtual FOnBlinkingEndedSignature& ProvideOnBlinkingEndedDelegate() = 0;
};
