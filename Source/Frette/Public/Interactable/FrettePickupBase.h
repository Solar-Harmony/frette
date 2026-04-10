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
	virtual void OnConstruction(const FTransform& Transform) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn="true"))
	TObjectPtr<UFretteInventoryItemDataAsset> ItemData;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta=(ExposeOnSpawn="true"))
	bool bDestroyOnPickUp = true;
	
	UPROPERTY(EditInstanceOnly)
	TObjectPtr<UFretteInteractableComponent> Interactable;
	
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> StaticMesh;
	
	UFUNCTION(BlueprintNativeEvent)
	void OnPickUp(AFrettePlayerCharacter* Interactor, UFretteInventoryItem* AddedItem);

protected:
	virtual void BeginPlay() override;
	
	// overridable native implementation
	virtual void OnPickUp_Implementation(AFrettePlayerCharacter* Interactor, UFretteInventoryItem* AddedItem) {}
	
private:
	UFUNCTION()
	void OnInteractDelegate(AFrettePlayerCharacter* Interactor);
	
	UFUNCTION(Client, Reliable)
	void Client_OnInteract(AFrettePlayerCharacter* Interactor);
	
	UFUNCTION(Server, Reliable)
	void Server_OnInteract(AFrettePlayerCharacter* Interactor);
	
	void OnItemMeshLoaded(const FSoftObjectPath&, UObject* LoadedObject) const;
};