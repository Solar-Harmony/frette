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

public:
	virtual void Die() override;

protected:
	AFretteEnemyCharacter();
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void Multicast_HandleDeath_Implementation(FVector FinalVelocity) override;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UPawnSensingComponent> AiPawnSensing;
};