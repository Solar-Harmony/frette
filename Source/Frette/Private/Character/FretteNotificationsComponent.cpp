#include "Character/FretteNotificationsComponent.h"

#include "GameFramework/PlayerController.h"

void UFretteNotificationsComponent::Notify(const AFrettePlayerCharacter* PlayerCharacter, const FText& Message)
{
	PlayerCharacter->GetNotifications()->Send(Message);
}

void UFretteNotificationsComponent::NotifyAll(const UObject* WorldContextObject, const FText& Message)
{
	if (!IsValid(WorldContextObject))
		return;

	const UWorld* World = WorldContextObject->GetWorld();
	if (World->GetNetMode() == NM_Client)
		return;

	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		const APlayerController* Controller = It->Get();
		const AFrettePlayerCharacter* PlayerCharacter =  Controller->GetPawn<AFrettePlayerCharacter>();
		if (PlayerCharacter == nullptr)
			continue;

		PlayerCharacter->GetNotifications()->Send(Message);
	}
}

void UFretteNotificationsComponent::Send_Implementation(const FText& Message)
{
	PendingMessages.Enqueue(Message);
	OnAddNotification.ExecuteIfBound(Message);
	
	FTimerHandle _;
	const FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &UFretteNotificationsComponent::PopNotification);
	GetWorld()->GetTimerManager().SetTimer(_, Delegate, NotificationFadeDelay, false);
}


void UFretteNotificationsComponent::PopNotification()
{
	FText Message;
	PendingMessages.Dequeue(Message);
	OnRemoveNotification.ExecuteIfBound();
}