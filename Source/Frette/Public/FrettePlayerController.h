#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FrettePlayerController.generated.h"

UCLASS()
class FRETTE_API AFrettePlayerController : public APlayerController
{
	GENERATED_BODY()
	
private:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Frette")
	TObjectPtr<class UInputMappingContext> DefaultInputContext;
};
