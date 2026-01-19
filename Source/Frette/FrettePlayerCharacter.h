#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Components/WidgetComponent.h"

#include "FrettePlayerCharacter.generated.h"

class UAbilitySystemComponent;
class UFretteAttributeSet;

UCLASS()
class AFrettePlayerCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AFrettePlayerCharacter();
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void Tick(float DeltaTime) override;
	
	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UFUNCTION(BlueprintCallable)
	void DoPlayerMove(FVector2D MoveAxis);
	
	UFUNCTION(BlueprintCallable)
	void DoPlayerLook(FVector2D LookAxis);
	
	UFUNCTION(BlueprintCallable)
	void DoPlayerJump();
	
	UFUNCTION(BlueprintCallable)
	void DoPlayerSprint(bool bIsSprinting);

	virtual void Landed(const FHitResult& Hit) override;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetHealth() const;
	
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxHealth() const;
	
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetStamina() const;
	
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxStamina() const;
	
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetHealthPercent() const;
	
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetStaminaPercent() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UFretteAttributeSet> AttributeSet;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> HealthWidgetComponent;

	void InitializeAbilitySystem();
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintSpeedMultiplier = 2.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float StaminaDrainRate = 10.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float StaminaRegenRate = 20.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float FallDamageThreshold = 1000.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float FallDamageMultiplier = 0.01f;

	bool bIsSprinting = false;

private:
	void HandleStaminaDrain(float DeltaTime);
};
