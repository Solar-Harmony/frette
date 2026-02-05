#pragma once

#include "CoreMinimal.h"
#include "FakeInventoryComponent.h"
#include "FretteBaseCharacter.h"
#include "Camera/CameraComponent.h"

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

	virtual void GetAimData(FVector& OutLocation, FRotator& OutRotation) override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UFakeInventoryComponent> InventoryComponent;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> Camera;

private:
	virtual void InitAbilityActorInfo() override;
};