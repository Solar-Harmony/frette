#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "FrettePlayerCharacter.h"
#include "Character/FretteBaseCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "FretteEnemyCharacter.generated.h"

class UPawnSensingComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDied, AFretteEnemyCharacter*, Character);

UCLASS()
class FRETTE_API AFretteEnemyCharacter : public AFretteBaseCharacter
{
	GENERATED_BODY()

public:
	virtual void Die() override;

	UPROPERTY()
	FOnEnemyDied OnDied;

	FVector PatrolOrigin;

	float PatrolRadius = 1000.f;

	UPROPERTY(BlueprintReadWrite, Replicated)
	bool WaitingAtPatrolPoint = false;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AFrettePlayerCharacter> TargetedPlayer;

protected:
	AFretteEnemyCharacter();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void Multicast_HandleDeath_Implementation(FVector FinalVelocity) override;

	UFUNCTION()
	void OnTargetPerceived(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerPerceived(AFrettePlayerCharacter* Player);

	UFUNCTION(BlueprintCallable)
	FVector GetRandomPatrolPoint() const;

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerLost(FVector LastKnownLocation);

	UPROPERTY(EditAnywhere, Category = "Frette|AI")
	TObjectPtr<UAIPerceptionComponent> AiPerception;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frette|AI")
	FVector2f PatrolWaitRandomInterval = FVector2f(4.f, 8.f);

	FTimerHandle PatrolTimerHandle;
};