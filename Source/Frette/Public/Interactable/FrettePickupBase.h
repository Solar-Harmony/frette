#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable/FretteInteractableInterface.h"
#include "Inventory/Items/FretteInventoryItem.h"
#include "FrettePickupBase.generated.h"

class AFrettePlayerCharacter;
class UFretteInteractableComponent;

/*
 * An interactible item in the world. 
 * Does nothing by default, subclass and implement OnPickUp or OnPickUp_Implementation to add functionality.
 */
UCLASS(Abstract)
class FRETTE_API AFrettePickupBase : public AActor, public IFretteInteractableInterface
{
	GENERATED_BODY()
	
public:	
	AFrettePickupBase();
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta=(ExposeOnSpawn="true"))
	TObjectPtr<UFretteInventoryItemDataAsset> ItemData;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta=(ExposeOnSpawn="true"))
	bool bDestroyOnPickUp = true;
	
	UPROPERTY(EditInstanceOnly)
	TObjectPtr<UFretteInteractableComponent> Interactable;
	
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> StaticMesh;
	
	UFUNCTION(BlueprintNativeEvent)
	void OnPickUp(AFrettePlayerCharacter* Interactor);
	
protected:
	// overridable native implementation
	virtual void OnPickUp_Implementation(AFrettePlayerCharacter* Interactor) {}
	
private:
	virtual void OnConstruction(const FTransform& Transform) override;
	
	UFUNCTION()
	void OnInteract_Internal(AFrettePlayerCharacter* Interactor);
	
	void OnItemMeshLoaded(const FSoftObjectPath&, UObject* LoadedObject);
};