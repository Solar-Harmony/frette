#pragma once

#include "CoreMinimal.h"
#include "Character/FrettePlayerCharacter.h"
#include "Inventory/FretteInventoryComponent.h"
#include "Inventory/Items/FretteInventoryItem.h"
#include "Inventory/Items/FretteSlottableItem.h"
#include "Player/FrettePlayerController.h"
#include "FretteClueItem.generated.h"

UCLASS(BlueprintType)
class UFretteClueItem : public UFretteSlottableItem
{
	GENERATED_BODY()

public:
	virtual void Use_Implementation() override
	{
		const UFretteInventoryComponent* Inventory = GetOwningInventory();
		const AFrettePlayerCharacter* PlayerCharacter = Cast<AFrettePlayerCharacter>(Inventory->GetOwner());
		check(PlayerCharacter);
		const AFrettePlayerController* PlayerController = Cast<AFrettePlayerController>(PlayerCharacter->GetController());
		check(PlayerController);
		PlayerController->OnClientReceiveNewClue.Broadcast(ClueText);
	}

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, meta = (ExposeOnSpawn = true))
	FText ClueText;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(ThisClass, ClueText);
	}
};

UCLASS()
class UFretteClueItemDataAsset : public UFretteSlottableDataAsset
{
	GENERATED_BODY()
	
	virtual const UClass* GetRuntimeItemClass() const override { return UFretteClueItem::StaticClass(); }
};