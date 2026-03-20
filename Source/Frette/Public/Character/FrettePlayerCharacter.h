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
class UFretteInventoryComponent;
class UFretteNotificationsComponent;

UCLASS()
class AFrettePlayerCharacter : public AFretteBaseCharacter
{
	GENERATED_BODY()

public:
	UFretteEquipmentComponent* GetEquipmentComponent() const { return EquipmentComponent; }
	
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
	
	UFretteNotificationsComponent* GetNotifications() const { return NotificationsComponent; }

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
	float LookSmoothingSpeed = 20.0f;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UFretteEquipmentComponent> EquipmentComponent;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFretteInventoryComponent> PlayerInventory;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFretteNotificationsComponent> NotificationsComponent;

private:
	virtual void InitAbilityActorInfo() override;
};