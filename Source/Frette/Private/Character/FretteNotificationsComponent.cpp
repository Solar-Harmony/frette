#include "Character/FretteNotificationsComponent.h"

void UFretteNotificationsComponent::Notify(const AFrettePlayerCharacter* PlayerCharacter, const FText& Message)
{
	PlayerCharacter->GetNotifications()->Send(Message);
}

void UFretteNotificationsComponent::NotifyAll(const UObject* WorldContextObject, const FText& Message)
{
	const AFrettePlayerCharacter* PlayerCharacter = WorldContextObject->GetWorld()->GetFirstPlayerController()->GetPawn<AFrettePlayerCharacter>();
	PlayerCharacter->GetNotifications()->SendToEveryone(Message);
}

void UFretteNotificationsComponent::Send_Implementation(const FText& Message)
{
	PendingMessages.Enqueue(Message);
	OnAddNotification.ExecuteIfBound(Message);
	
	FTimerHandle _;
	const FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &UFretteNotificationsComponent::PopNotification);
	GetWorld()->GetTimerManager().SetTimer(_, Delegate, NotificationFadeDelay, false);
}

void UFretteNotificationsComponent::SendToEveryone_Implementation(const FText& Message)
{
	Send_Implementation(Message);
}

void UFretteNotificationsComponent::PopNotification()
{
	FText Message;
	PendingMessages.Dequeue(Message);
	OnRemoveNotification.ExecuteIfBound();
}