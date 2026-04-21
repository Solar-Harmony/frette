#include "FretteEditor.h"

#include "FretteClueTemplateCustomization.h"
#include "MessageLogModule.h"
#include "CoreGameplay/FretteClueTemplateSet.h"

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

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout(
		FFretteClueTemplate::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FFretteClueTemplateCustomization::MakeInstance)
	);
	PropertyModule.NotifyCustomizationModuleChanged();
}

void FFretteEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomPropertyTypeLayout(FFretteClueTemplate::StaticStruct()->GetFName());
	}
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FFretteEditorModule, FretteEditor)