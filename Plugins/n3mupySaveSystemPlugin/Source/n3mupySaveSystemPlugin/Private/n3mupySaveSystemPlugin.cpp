// Copyright n3mupy

#include "n3mupySaveSystemPlugin.h"

#define LOCTEXT_NAMESPACE "Fn3mupySaveSystemPluginModule"

void Fn3mupySaveSystemPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void Fn3mupySaveSystemPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(Fn3mupySaveSystemPluginModule, n3mupySaveSystemPlugin)