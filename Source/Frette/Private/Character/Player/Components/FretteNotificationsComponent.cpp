#include "Character/Player/Components/FretteNotificationsComponent.h"

#include "Frette.h"
#include "GameFramework/PlayerController.h"

void UFretteNotificationsComponent::Notify(const AFrettePlayerCharacter* PlayerCharacter, const FText& Message)
{
	unless(PlayerCharacter != nullptr, "Tried to send notification on null player character")
		return;
	
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
	if (PendingMessages.ContainsByPredicate([&](const FText& PendingMessage) { return PendingMessage.EqualTo(Message); }))
		return;
	
	PendingMessages.Add(Message);
	OnAddNotification.ExecuteIfBound(Message);
	
	FTimerHandle _;
	const FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &UFretteNotificationsComponent::PopNotification);
	GetWorld()->GetTimerManager().SetTimer(_, Delegate, NotificationFadeDelay, false);
}


void UFretteNotificationsComponent::PopNotification()
{
	FText Message = PendingMessages[0];
	PendingMessages.RemoveAtSwap(0);
	
	OnRemoveNotification.ExecuteIfBound();
}