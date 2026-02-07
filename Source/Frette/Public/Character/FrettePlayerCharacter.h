#pragma once

#include "CoreMinimal.h"
#include "FretteBaseCharacter.h"
#include "Inventory/StacksInventoryComponent.h"
#include "FrettePlayerCharacter.generated.h"

class UCameraComponent;
class UInventoryComponent;

UCLASS()
class AFrettePlayerCharacter : public AFretteBaseCharacter
{
	GENERATED_BODY()

public:
	AFrettePlayerCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	UFUNCTION(BlueprintCallable)
	void DoPlayerMove(FVector2D MoveAxis);

	UFUNCTION(BlueprintCallable)
	void DoPlayerLook(FVector2D LookAxis);

	UFUNCTION(BlueprintCallable)
	void DoPlayerJump();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> Camera;

private:
	virtual void InitAbilityActorInfo() override;
};