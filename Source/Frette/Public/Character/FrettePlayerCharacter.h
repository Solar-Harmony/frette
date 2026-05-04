#pragma once

#include "CoreMinimal.h"
#include "FretteBaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/FretteMantlingAbility.h"

#include "Equipment/FretteEquipmentComponent.h"
#include "FrettePlayerCharacter.generated.h"

class UFretteCompassComponent;

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
	void SetupPlayerCollisions() const;
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	UFUNCTION(BlueprintCallable)
	void DoPlayerMove(FVector2D MoveAxis);

	UFUNCTION(BlueprintCallable)
	void DoPlayerLook(FVector2D LookAxis);

	UPROPERTY()
	FOnPlayerDied OnPlayerDied;

	virtual void Die() override;
	
	virtual void Revive() override;

	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult) override;

	UFUNCTION(BlueprintCallable)
	UFretteInventoryComponent* GetPlayerInventory() const { return PlayerInventory; }
	
	UFUNCTION(BlueprintCallable)
	void HandleRagdoll(FVector DeathVelocity);
	void RecoverFromRagdoll();

	UFretteNotificationsComponent* GetNotifications() const { return NotificationsComponent; }

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="Frette|Look")
	float HorizontalLookSensitivity = 1.0f;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="Frette|Look")
	float VerticalLookSensitivity = 1.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UFretteCompassComponent> CompassComponent;
	
	UPROPERTY(EditDefaultsOnly)
	float ReviveTimer = 5.0f;

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

	virtual void Multicast_HandleDeath_Implementation(FVector FinalVelocity) override;
	virtual void Multicast_HandleRevive_Implementation() override;
	
	void UnRagdoll();

private:
	virtual void InitAbilityActorInfo() override;
};