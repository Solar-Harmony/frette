#pragma once

#include "CoreMinimal.h"
#include "FretteViewModel.h"
#include "InventoryItemVM.h"
#include "Character/FrettePlayerCharacter.h"
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
		const auto* PlayerCharacter = Cast<AFrettePlayerCharacter>(OwningController->GetPawn());
		check(PlayerCharacter);

		auto* Inventory = PlayerCharacter->GetComponentByClass<UInventoryComponent>();
		check(Inventory);

		// TODO: Add api to inventory
		// Inventory->OnItemAdded.AddDynamic(
		// 	this,
		// 	&UInventoryVM::AddItem
		// 	);
	}

	UFUNCTION(BlueprintCallable)
	void AddItem(const FInventoryItem& NewItem)
	{
		UInventoryItemVM* SubViewModel = NewObject<UInventoryItemVM>(this);
		SubViewModel->SetName(NewItem);
		Items.Add(SubViewModel);

		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Items);
	}
};