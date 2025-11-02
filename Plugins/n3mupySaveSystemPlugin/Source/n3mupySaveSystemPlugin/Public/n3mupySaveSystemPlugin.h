// Copyright n3mupy

#pragma once

#include "Modules/ModuleManager.h"

class Fn3mupySaveSystemPluginModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
