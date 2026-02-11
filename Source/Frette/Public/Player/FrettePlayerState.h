#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"

#include "GameFramework/PlayerState.h"
#include "Inventory/InventoryComponent.h"
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
	UFretteInventoryComponent* GetPlayerInventory() const { return PlayerInventory; }

protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(Replicated)
	TObjectPtr<UFretteInventoryComponent> PlayerInventory;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
};