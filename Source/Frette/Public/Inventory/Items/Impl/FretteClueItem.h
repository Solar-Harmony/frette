#pragma once

#include "CoreMinimal.h"
#include "Character/FrettePlayerCharacter.h"
#include "Inventory/FretteInventoryComponent.h"
#include "Inventory/Items/FretteInventoryItem.h"
#include "Player/FrettePlayerController.h"
#include "FretteClueItem.generated.h"

class UFretteClueItemDataAsset;

UCLASS(BlueprintType)
class UFretteClueItem : public UFretteInventoryItem
{
	GENERATED_BODY()
	FRETTE_ITEM_DATA_GETTER(UFretteClueItemDataAsset)

public:
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, meta = (ExposeOnSpawn = true))
	bool bIsPrimary;
	
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, meta = (ExposeOnSpawn = true))
	FText ClueText;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(ThisClass, ClueText);
		DOREPLIFETIME(ThisClass, bIsPrimary);
	}
	
private:
	virtual void Use_Implementation() override
	{
		const UFretteInventoryComponent* Inventory = GetOwningInventory();
		const AFrettePlayerCharacter* PlayerCharacter = Cast<AFrettePlayerCharacter>(Inventory->GetOwner());
		check(PlayerCharacter);
		const AFrettePlayerController* PlayerController = Cast<AFrettePlayerController>(PlayerCharacter->GetController());
		check(PlayerController);
		PlayerController->Client_OnClueGenerated(ClueText);
	}
};

UCLASS()
class UFretteClueItemDataAsset : public UFretteInventoryItemDataAsset
{
	GENERATED_BODY()
	
public:
	virtual const UClass* GetRuntimeItemClass() const override { return UFretteClueItem::StaticClass(); }
};