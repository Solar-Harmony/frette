#include "Player/FrettePlayerState.h"

#include "GameplayAbilitySystem/FretteAbilitySystemComponent.h"
#include "GameplayAbilitySystem/FretteAttributeSet.h"

AFrettePlayerState::AFrettePlayerState()
{
	NonStackableInventory = CreateDefaultSubobject<USlotsInventoryComponent>("Equipment Inventory");
	StackableInventory = CreateDefaultSubobject<UStacksInventoryComponent>("Backpack Inventory");

	AbilitySystemComponent = CreateDefaultSubobject<UFretteAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);

	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UFretteAttributeSet>("AttributeSet");

	//How often the server will try to update the clients in a second?
	//Doit être augmenter ici car de base le update frequency du playerState est très bas
	SetNetUpdateFrequency(100.f);
}

UAbilitySystemComponent* AFrettePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}