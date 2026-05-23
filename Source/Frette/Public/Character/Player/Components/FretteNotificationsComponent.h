#pragma once

#include "CoreMinimal.h"
#include "Character/Player/FrettePlayerCharacter.h"
#include "FretteNotificationsComponent.generated.h"



DECLARE_DELEGATE_OneParam(FOnAddPlayerNotification, const FText& /*NotificationText*/);
DECLARE_DELEGATE(FOnRemovePlayerNotification);

// Allows sending visual notifications to clients
UCLASS()
class UFretteNotificationsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void Notify(const AFrettePlayerCharacter* PlayerCharacter, const FText& Message);
	
	UFUNCTION(BlueprintCallable, meta=(WorldContext = "WorldContextObject"))
	static void NotifyAll(const UObject* WorldContextObject, const FText& Message);
	
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void Send(const FText& Message);
	
	// Callbacks for UI viewmodel to bind to
	FOnAddPlayerNotification OnAddNotification;
	FOnRemovePlayerNotification OnRemoveNotification;
	
private:
	UPROPERTY(EditAnywhere, meta = (Units = "s"))
	float NotificationFadeDelay = 5.0f;
	
	void PopNotification();
	
	TArray<FText> PendingMessages;
};