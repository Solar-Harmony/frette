#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Inventory/Items/FretteSlottableItem.h"
#include "FretteEquipmentComponent.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_BodyPart_Head);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_BodyPart_Chest);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_BodyPart_Legs);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_BodyPart_Feet);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_BodyPart_Hand);

class AFrettePlayerState;

UCLASS()
class FRETTE_API UFretteEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(Server, Reliable)
	void EquipItem(UFretteSlottableItem* Item) const;
	
	UFUNCTION(Server, Reliable)
	void UnequipItem(UFretteSlottableItem* Item) const;
};