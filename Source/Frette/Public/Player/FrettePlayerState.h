#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"

#include "GameFramework/PlayerState.h"
#include "Inventory/SlotsInventoryComponent.h"
#include "FrettePlayerState.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;

UCLASS()
class FRETTE_API AFrettePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AFrettePlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
	UFUNCTION(BlueprintCallable)
	USlotsInventoryComponent* GetPlayerInventory() const { return PlayerInventory; }

protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(Replicated)
	TObjectPtr<USlotsInventoryComponent> PlayerInventory;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
};