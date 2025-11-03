// Copyright Offmeta

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BlinkingProviderInterface.generated.h"

class UEyesightOverlayWidget;
DECLARE_MULTICAST_DELEGATE(FOnBlinkingEndedSignature)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEyesightOverlayReadySignature, UEyesightOverlayWidget* /** Widget */)

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
	virtual FOnEyesightOverlayReadySignature& ProvideOnEyesightOverlayReadyDelegate() = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UEyesightOverlayWidget* GetEyesightOverlayWidget();
};
