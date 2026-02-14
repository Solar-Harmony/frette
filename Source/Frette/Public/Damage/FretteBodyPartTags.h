#pragma once
#include "FretteBodyPartDefinitions.h"
#include "NativeGameplayTags.h"

class UFretteBodyPartAttributeSet;

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_BodyPart_Head);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_BodyPart_Torso);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_BodyPart_LeftArm);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_BodyPart_RightArm);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_BodyPart_LeftHand);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_BodyPart_RightHand);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_BodyPart_LeftLeg);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_BodyPart_RightLeg);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_BodyPart_LeftFoot);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_BodyPart_RightFoot);

namespace Frette::BodyParts
{
	using FTagToAttributeSetMap = TMap<FGameplayTag, TSubclassOf<UFretteBodyPartAttributeSet>>;
	extern FTagToAttributeSetMap TagToAttributeSetMap;
	
	static const UFretteBodyPartAttributeSet* GetAttributeSetFromTag(UAbilitySystemComponent* ASC, FGameplayTag BodyPartTag)
	{
		if (UNLIKELY(TagToAttributeSetMap.Num() == 0))
		{
			TagToAttributeSetMap.Add(TAG_BodyPart_Head, UFretteBodyPartAttributeSet_Head::StaticClass());
			TagToAttributeSetMap.Add(TAG_BodyPart_Torso, UFretteBodyPartAttributeSet_Torso::StaticClass());
			TagToAttributeSetMap.Add(TAG_BodyPart_LeftArm, UFretteBodyPartAttributeSet_LeftArm::StaticClass());
			TagToAttributeSetMap.Add(TAG_BodyPart_RightArm, UFretteBodyPartAttributeSet_RightArm::StaticClass());
			TagToAttributeSetMap.Add(TAG_BodyPart_LeftHand, UFretteBodyPartAttributeSet_LeftHand::StaticClass());
			TagToAttributeSetMap.Add(TAG_BodyPart_RightHand, UFretteBodyPartAttributeSet_RightHand::StaticClass());
			TagToAttributeSetMap.Add(TAG_BodyPart_LeftLeg, UFretteBodyPartAttributeSet_LeftLeg::StaticClass());
			TagToAttributeSetMap.Add(TAG_BodyPart_RightLeg, UFretteBodyPartAttributeSet_RightLeg::StaticClass());
			TagToAttributeSetMap.Add(TAG_BodyPart_LeftFoot, UFretteBodyPartAttributeSet_LeftFoot::StaticClass());
			TagToAttributeSetMap.Add(TAG_BodyPart_RightFoot, UFretteBodyPartAttributeSet_RightFoot::StaticClass());
		}

		const TSubclassOf<UFretteBodyPartAttributeSet> BodyPartAttributeSetClass = TagToAttributeSetMap[BodyPartTag];
		const TSubclassOf<UAttributeSet> AttributeSetClass = StaticCast<TSubclassOf<UAttributeSet>>(BodyPartAttributeSetClass);
		const UAttributeSet* AttributeSet = ASC->GetAttributeSet(AttributeSetClass);
		const UFretteBodyPartAttributeSet* BodyPartAttributeSet = Cast<UFretteBodyPartAttributeSet>(AttributeSet);
		return BodyPartAttributeSet;
	}
}