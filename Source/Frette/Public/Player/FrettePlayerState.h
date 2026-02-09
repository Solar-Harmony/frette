#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"

#include "GameFramework/PlayerState.h"
#include "Inventory/SlotsInventoryComponent.h"
#include "Inventory/StacksInventoryComponent.h"
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
	UStacksInventoryComponent* GetStacksInventory() const { return StackableInventory; }

	UFUNCTION(BlueprintCallable)
	USlotsInventoryComponent* GetSlotsInventory() const { return NonStackableInventory; }

protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(Replicated)
	TObjectPtr<UStacksInventoryComponent> StackableInventory;

	UPROPERTY(Replicated)
	TObjectPtr<USlotsInventoryComponent> NonStackableInventory;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
};