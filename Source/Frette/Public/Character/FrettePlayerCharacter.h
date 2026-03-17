#pragma once

#include "CoreMinimal.h"
#include "FretteBaseCharacter.h"
#include "Camera/CameraComponent.h"

#include "Equipment/FretteEquipmentComponent.h"
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

	UFUNCTION(BlueprintCallable)
	UFretteInventoryComponent* GetPlayerInventory() const { return PlayerInventory; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMeshComponent> FPMesh;

	virtual void BeginPlay() override;
	void SetLookInputScale();

	UPROPERTY(EditAnywhere)
	float HorizontalSensitivity = 1.f;

	UPROPERTY(EditAnywhere)
	float VerticalSensitivity = 1.f;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFretteInventoryComponent> PlayerInventory;

private:
	virtual void InitAbilityActorInfo() override;
};