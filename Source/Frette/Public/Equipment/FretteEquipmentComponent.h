#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Inventory/FretteInventoryComponent.h"
#include "Inventory/Items/FretteInventoryItem.h"
#include "FretteEquipmentComponent.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_EquipmentSlot_Head);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_EquipmentSlot_Chest);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_EquipmentSlot_Legs);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_EquipmentSlot_Feet);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_EquipmentSlot_Hand);

class AFrettePlayerState;

UCLASS()
class FRETTE_API UFretteEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void Initialize(UFretteInventoryComponent* PlayerState);

private:
	void OnInventoryItemSelected(const UFretteInventoryItem* Item);
};