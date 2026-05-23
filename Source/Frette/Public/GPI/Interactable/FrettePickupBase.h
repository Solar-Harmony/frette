#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GPI/Interactable/FretteInteractableInterface.h"
#include "Inventory/Items/FretteInventoryItem.h"
#include "FrettePickupBase.generated.h"

class AFrettePlayerCharacter;
class UFretteInteractableComponent;

/*
 * An interactible that creates an item and adds it to the interactor's inventory.
 */
UCLASS()
class FRETTE_API AFrettePickupBase : public AActor, public IFretteInteractableInterface
{
	GENERATED_BODY()
	
public:	
	AFrettePickupBase();
	virtual void OnConstruction(const FTransform& Transform) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_ItemData, meta=(ExposeOnSpawn="true"))
	TObjectPtr<UFretteInventoryItemDataAsset> ItemData;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn="true"))
	bool bHasPhysics = true;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta=(ExposeOnSpawn="true"))
	bool bDestroyOnPickUp = true;
	
	UPROPERTY(EditInstanceOnly)
	TObjectPtr<UFretteInteractableComponent> Interactable;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> StaticMesh;
	
	UFUNCTION(BlueprintNativeEvent)
	void OnPickUp(AFrettePlayerCharacter* Interactor, UFretteInventoryItem* AddedItem);

protected:
	virtual void BeginPlay() override;
	
	// overridable native implementation
	virtual void OnPickUp_Implementation(AFrettePlayerCharacter* Interactor, UFretteInventoryItem* AddedItem) {}
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	UFUNCTION()
	void OnInteractDelegate(AFrettePlayerCharacter* Interactor);
	
	UFUNCTION(Client, Reliable)
	void Client_OnInteract(AFrettePlayerCharacter* Interactor);
	
	UFUNCTION(Server, Reliable)
	void Server_OnInteract(AFrettePlayerCharacter* Interactor);
	
	UFUNCTION()
	void OnRep_ItemData();


	void UpdateVisuals();

	void OnItemMeshLoaded(const FSoftObjectPath&, UObject* LoadedObject) const;
};