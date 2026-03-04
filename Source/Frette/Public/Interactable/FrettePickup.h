#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable/FretteInteractableInterface.h"
#include "Inventory/Items/FretteInventoryItem.h"
#include "FrettePickup.generated.h"

class UFretteInteractableComponent;

UCLASS()
class FRETTE_API AFrettePickup : public AActor, public IFretteInteractableInterface
{
	GENERATED_BODY()
	
public:	
	AFrettePickup();
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta=(ExposeOnSpawn="true"))
	TObjectPtr<UFretteInventoryItemDataAsset> ItemData;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta=(ExposeOnSpawn="true"))
	bool bDestroyOnPickUp = true;
	
	UPROPERTY(EditInstanceOnly)
	TObjectPtr<UFretteInteractableComponent> Interactable;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMesh;
	
	UFUNCTION(BlueprintNativeEvent)
	void OnInteract();
	
protected:
	// overridable native implementation
	virtual void OnInteract_Implementation() {}
	
private:
	virtual void OnConstruction(const FTransform& Transform) override;
	
	UFUNCTION()
	void OnInteract_Internal();
	
	void OnItemMeshLoaded(const FSoftObjectPath&, UObject* LoadedObject);
};