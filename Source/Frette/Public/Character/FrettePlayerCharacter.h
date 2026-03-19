#pragma once

#include "CoreMinimal.h"
#include "FretteBaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/BodyPart/FretteBodyPartComponent.h"

#include "Equipment/FretteEquipmentComponent.h"
#include "FrettePlayerCharacter.generated.h"

class UFretteTemperatureComponent;

class UCameraComponent;
class UInventoryComponent;

UCLASS()
class AFrettePlayerCharacter : public AFretteBaseCharacter
{
	GENERATED_BODY()

public:
	UFretteEquipmentComponent* GetEquipmentComponent() const { return Equipment; }
	
	AFrettePlayerCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	UFUNCTION(BlueprintCallable)
	void DoPlayerMove(FVector2D MoveAxis);

	UFUNCTION(BlueprintCallable)
	void DoPlayerLook(FVector2D LookAxis);

	UFUNCTION(BlueprintCallable)
	void DoPlayerJump();

	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult) override;

	UFUNCTION(BlueprintCallable)
	UFretteInventoryComponent* GetPlayerInventory() const { return PlayerInventory; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UFretteBodyPartComponent> BodyPartComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UFretteTemperatureComponent> BodyTemperatureComponent;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMeshComponent> FPMesh;

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	FRotator SmoothedControlRotation;

	UPROPERTY(EditDefaultsOnly, Category="Frette|Look")
	float LookSmoothingSpeed = 20.f;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFretteEquipmentComponent> Equipment;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFretteInventoryComponent> PlayerInventory;

private:
	virtual void InitAbilityActorInfo() override;
};