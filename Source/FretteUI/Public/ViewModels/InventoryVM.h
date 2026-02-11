#pragma once

#include "CoreMinimal.h"
#include "FretteViewModel.h"
#include "InventoryItemVM.h"
#include "Character/FrettePlayerCharacter.h"
#include "Inventory/InventoryComponent.h"
#include "Player/FrettePlayerState.h"
#include "InventoryVM.generated.h"

UCLASS()
class FRETTEUI_API UInventoryVM : public UFretteViewModel
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TArray<TObjectPtr<UInventoryItemVM>> Items;

	virtual void Bind(APlayerController* OwningController) override
	{
		if (const AFrettePlayerState* State = OwningController->GetPlayerState<AFrettePlayerState>())
		{
			UFretteInventoryComponent* Inventory = State->GetPlayerInventory();
			Inventory->OnItemAdded.AddUObject(this, &UInventoryVM::AddItem);
		}
	}

	UFUNCTION(BlueprintCallable)
	void AddItem(UInventoryItem* NewItem)
	{
		UInventoryItemVM* SubViewModel = NewObject<UInventoryItemVM>(this);
		SubViewModel->SetName(NewItem);
		Items.Add(SubViewModel);

		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Items);
	}
};