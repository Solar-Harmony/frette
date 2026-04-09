#pragma once

#include "CoreMinimal.h"
#include "Character/FretteBaseCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "FretteEnemyCharacter.generated.h"

class UPawnSensingComponent;
/**
 * 
 */
UCLASS()
class FRETTE_API AFretteEnemyCharacter : public AFretteBaseCharacter
{
	GENERATED_BODY()

protected:
	AFretteEnemyCharacter();
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAIPerceptionComponent> AiPerceptionComponent;
};