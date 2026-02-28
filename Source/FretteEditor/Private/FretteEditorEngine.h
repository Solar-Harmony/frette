#pragma once

#include "CoreMinimal.h"
#include "Editor/UnrealEdEngine.h"
#include "FretteEditorEngine.generated.h"

/**
 * Used to register status branches to the editor
 * Basically it's so the editor can detect when we do a change that diverges with master
 */
UCLASS()
class FRETTEEDITOR_API UFretteEditorEngine : public UUnrealEdEngine
{
	GENERATED_BODY()
	
	virtual void Init(IEngineLoop* InEngineLoop) override;
};