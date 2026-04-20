#pragma once

#include "CoreMinimal.h"
#include "FrettePlayerState.h"
#include "GameplayTagContainer.h"
#include "InputAction.h"
#include "GameFramework/PlayerController.h"
#include "GameplayAbilitySystem/FretteAbilitySystemComponent.h"
#include "Input/FretteInputConfig.h"
#include "Interactable/FretteInteractorComponent.h"
#include "FrettePlayerController.generated.h"

class UCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClientReceiveNewClue, const FText&, ClueText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWidgetsInitialized);

UCLASS()
class FRETTE_API AFrettePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnClientReceiveNewClue OnClientReceiveNewClue;

	UFUNCTION(Client, Reliable)
	void Client_OnClueGenerated(const FText& ClueText) const;
	
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Frette|Calisse")
	void SetFretteCinematicMode(bool bIsCinematic);
	
	UPROPERTY()
	bool bFretteCinematicMode = false;
	
	// Only runs on clients.
	UPROPERTY(BlueprintAssignable, meta=(DisplayName="Event Widgets Created"))
	FOnWidgetsInitialized OnWidgetsInitialized;
	
protected:
	virtual void SetupInputComponent() override;
	void ToggleSettingsMenu();
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnRep_PlayerState() override;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Create Widgets and Viewmodels"))
	void SetupWidgetsAndViewModels();
	
private:
	AFrettePlayerController();

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Frette|HUD")
	TSubclassOf<AHUD> HUDClass;

	UPROPERTY(EditDefaultsOnly, Category = "Frette")
	TObjectPtr<class UInputMappingContext> DefaultInputContext;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category="Frette|Input")
	TObjectPtr<UFretteInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UFretteAbilitySystemComponent> FretteAbilitySystemComponent;

	UFUNCTION(BlueprintPure, Category = "Frette")
	AFrettePlayerState* GetFrettePlayerState() const { return GetPlayerState<AFrettePlayerState>(); }

	UFretteAbilitySystemComponent* GetASC();

	UPROPERTY(EditDefaultsOnly, Category="Frette|Input")
	TObjectPtr<UInputAction> InteractAction;
	
	UPROPERTY(EditDefaultsOnly, Category="Frette")
	TSubclassOf<UUserWidget> PauseMenuClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> PauseMenuInstance;
	
	UPROPERTY(EditDefaultsOnly, Category="Frette|Input")
	TObjectPtr<UInputAction> ToggleSettingAction;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UFretteInteractorComponent> Interactor;

	void OnInteractPressed();
	void OnInteractReleased();
};