#pragma once

#include "CoreMinimal.h"
#include "FretteBaseCharacter.h"
#include "Camera/CameraComponent.h"

#include "Equipment/FretteEquipmentComponent.h"
#include "FrettePlayerCharacter.generated.h"

class UFretteTemperatureComponent;

class UCameraComponent;
class UInventoryComponent;
class UFretteInventoryComponent;
class UFretteNotificationsComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDied, AFrettePlayerCharacter*, Character);

UCLASS()
class AFrettePlayerCharacter : public AFretteBaseCharacter
{
	GENERATED_BODY()

public:
	UFretteEquipmentComponent* GetEquipmentComponent() const { return EquipmentComponent; }

	AFrettePlayerCharacter();
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	UFUNCTION(BlueprintCallable)
	void DoPlayerMove(FVector2D MoveAxis);

	UFUNCTION(BlueprintCallable)
	void DoPlayerLook(FVector2D LookAxis);

	UFUNCTION(BlueprintCallable)
	void DoPlayerJump();

	bool GetIsDead() const { return bIsDead; }

	UPROPERTY()
	FOnPlayerDied OnPlayerDied;

	virtual void Die() override;

	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult) override;

	UFUNCTION(BlueprintCallable)
	UFretteInventoryComponent* GetPlayerInventory() const { return PlayerInventory; }

	UFretteNotificationsComponent* GetNotifications() const { return NotificationsComponent; }

protected:
	virtual void Tick(float DeltaSeconds) override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMeshComponent> FPMesh;

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

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_HandleDeath(FVector DeathVelocity);

	UPROPERTY(EditAnywhere)
	bool bIsDead = false;

private:
	virtual void InitAbilityActorInfo() override;
};