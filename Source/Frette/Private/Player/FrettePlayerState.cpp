#include "Player/FrettePlayerState.h"

#include "GameplayAbilitySystem/FretteAbilitySystemComponent.h"
#include "GameplayAbilitySystem/FretteAttributeSet.h"

AFrettePlayerState::AFrettePlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UFretteAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);

	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UFretteAttributeSet>("AttributeSet");
	
	AttributeSet_BodyPart_Head =
	CreateDefaultSubobject<UFretteBodyPartAttributeSet_Head>(TEXT("AttributeSet_BodyPart_Head"));

	AttributeSet_BodyPart_Torso =
		CreateDefaultSubobject<UFretteBodyPartAttributeSet_Torso>(TEXT("AttributeSet_BodyPart_Torso"));

	AttributeSet_BodyPart_LeftArm =
		CreateDefaultSubobject<UFretteBodyPartAttributeSet_LeftArm>(TEXT("AttributeSet_BodyPart_LeftArm"));

	AttributeSet_BodyPart_RightArm =
		CreateDefaultSubobject<UFretteBodyPartAttributeSet_RightArm>(TEXT("AttributeSet_BodyPart_RightArm"));

	AttributeSet_BodyPart_LeftHand =
		CreateDefaultSubobject<UFretteBodyPartAttributeSet_LeftHand>(TEXT("AttributeSet_BodyPart_LeftHand"));

	AttributeSet_BodyPart_RightHand =
		CreateDefaultSubobject<UFretteBodyPartAttributeSet_RightHand>(TEXT("AttributeSet_BodyPart_RightHand"));

	AttributeSet_BodyPart_LeftLeg =
		CreateDefaultSubobject<UFretteBodyPartAttributeSet_LeftLeg>(TEXT("AttributeSet_BodyPart_LeftLeg"));

	AttributeSet_BodyPart_RightLeg =
		CreateDefaultSubobject<UFretteBodyPartAttributeSet_RightLeg>(TEXT("AttributeSet_BodyPart_RightLeg"));

	AttributeSet_BodyPart_LeftFoot =
		CreateDefaultSubobject<UFretteBodyPartAttributeSet_LeftFoot>(TEXT("AttributeSet_BodyPart_LeftFoot"));

	AttributeSet_BodyPart_RightFoot =
		CreateDefaultSubobject<UFretteBodyPartAttributeSet_RightFoot>(TEXT("AttributeSet_BodyPart_RightFoot"));

	//How often the server will try to update the clients in a second?
	//Doit être augmenter ici car de base le update frequency du playerState est très bas
	SetNetUpdateFrequency(100.f);
}

UAbilitySystemComponent* AFrettePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}