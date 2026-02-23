#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Damage/FretteBodyPartDefinitions.h"

#include "GameFramework/PlayerState.h"
#include "FrettePlayerState.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;

UCLASS()
class FRETTE_API AFrettePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AFrettePlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet*                   GetAttributeSet() const { return AttributeSet; }

protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
	
	// body parts
	UPROPERTY()
	TObjectPtr<UFretteBodyPartAttributeSet_Head> AttributeSet_BodyPart_Head;

	UPROPERTY()
	TObjectPtr<UFretteBodyPartAttributeSet_Torso> AttributeSet_BodyPart_Torso;

	UPROPERTY()
	TObjectPtr<UFretteBodyPartAttributeSet_LeftArm> AttributeSet_BodyPart_LeftArm;

	UPROPERTY()
	TObjectPtr<UFretteBodyPartAttributeSet_RightArm> AttributeSet_BodyPart_RightArm;

	UPROPERTY()
	TObjectPtr<UFretteBodyPartAttributeSet_LeftHand> AttributeSet_BodyPart_LeftHand;

	UPROPERTY()
	TObjectPtr<UFretteBodyPartAttributeSet_RightHand> AttributeSet_BodyPart_RightHand;

	UPROPERTY()
	TObjectPtr<UFretteBodyPartAttributeSet_LeftLeg> AttributeSet_BodyPart_LeftLeg;

	UPROPERTY()
	TObjectPtr<UFretteBodyPartAttributeSet_RightLeg> AttributeSet_BodyPart_RightLeg;

	UPROPERTY()
	TObjectPtr<UFretteBodyPartAttributeSet_LeftFoot> AttributeSet_BodyPart_LeftFoot;

	UPROPERTY()
	TObjectPtr<UFretteBodyPartAttributeSet_RightFoot> AttributeSet_BodyPart_RightFoot;
};