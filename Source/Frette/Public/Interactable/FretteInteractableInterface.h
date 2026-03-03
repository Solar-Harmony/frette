#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FretteInteractableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UFretteInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class FRETTE_API IFretteInteractableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual UFretteInteractableComponent* GetInteractableComponent() { return nullptr; }
};
