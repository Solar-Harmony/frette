#pragma once

#include "CoreMinimal.h"
#include "FrettePlayerCharacter.h"
#include "FretteViewModel.h"
#include "InventoryComponent.h"
#include "InventoryViewModel.generated.h"

UCLASS()
class FRETTEUI_API UInventoryItemViewModel : public UFretteViewModel
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	FText Name;
	
	void SetName(const FInventoryItem& Item)
	{
		UE_MVVM_SET_PROPERTY_VALUE(Name, FText::FromName(Item.ItemName));
	}
};

UCLASS()
class FRETTEUI_API UInventoryViewModel : public UFretteViewModel
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	TArray<TObjectPtr<UInventoryItemViewModel>> Items;
	
	virtual void Bind(APlayerController* OwningController) override
	{
		const auto* PlayerCharacter = Cast<AFrettePlayerCharacter>(OwningController->GetPawn());
		check(PlayerCharacter);
		
		auto* Inventory = PlayerCharacter->GetComponentByClass<UInventoryComponent>();
		check(Inventory);
		
		Inventory->OnItemAdded.AddDynamic(
			this,
			&UInventoryViewModel::AddItem
		);
	}
	
	UFUNCTION(BlueprintCallable)
	void AddItem(const FInventoryItem& NewItem)
	{
		UInventoryItemViewModel* SubViewModel = NewObject<UInventoryItemViewModel>(this);
		SubViewModel->SetName(NewItem);
		Items.Add(SubViewModel);
		
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Items);
	}
};