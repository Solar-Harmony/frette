#pragma once

#include "CoreMinimal.h"
#include "FretteNotificationEntryVM.h"
#include "FretteViewModel.h"
#include "Character/Player/Components/FretteNotificationsComponent.h"
#include "FretteNotificationsVM.generated.h"

UCLASS()
class FRETTEUI_API UFretteNotificationsVM : public UFretteViewModel
{
	GENERATED_BODY()

public:
	virtual void Bind() override
	{
		UFretteNotificationsComponent* Notifications = PlayerCharacter->GetNotifications();
		Notifications->OnAddNotification.BindUObject(this, &UFretteNotificationsVM::EnqueueMessage);
		Notifications->OnRemoveNotification.BindUObject(this, &UFretteNotificationsVM::DequeueMessage);
	}

protected:
	UPROPERTY(FieldNotify, BlueprintReadOnly)
	TArray<UFretteNotificationEntryVM*> Messages;
	
	void EnqueueMessage(const FText& Text)
	{
		auto* Entry = NewObject<UFretteNotificationEntryVM>();
		Entry->SetMessage(Text);
		Messages.Insert(Entry, 0);
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Messages);
	}
	
	void DequeueMessage()
	{
		Messages.Pop();
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Messages);
	}
};