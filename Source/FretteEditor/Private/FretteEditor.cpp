#include "FretteEditor.h"

#include "MessageLogModule.h"

#define LOCTEXT_NAMESPACE "FFretteEditorModule"

void FFretteEditorModule::StartupModule()
{
	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");

	FMessageLogInitializationOptions InitOptions;
	InitOptions.bShowPages = true;

	MessageLogModule.RegisterLogListing(
		"FretteAssert",
		NSLOCTEXT("Frette", "FretteAssert", "Frette Assert"),
		InitOptions
	);
}

void FFretteEditorModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FFretteEditorModule, FretteEditor)