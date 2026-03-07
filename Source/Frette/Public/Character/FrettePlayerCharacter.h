#pragma once

#include "CoreMinimal.h"
#include "FakeInventoryComponent.h"
#include "FretteBaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/BodyPart/FretteBodyPartComponent.h"

#include "FrettePlayerCharacter.generated.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UFakeInventoryComponent> InventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UFretteBodyPartComponent> BodyPartComponent;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> Camera;

private:
	virtual void InitAbilityActorInfo() override;
};