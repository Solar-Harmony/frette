#include "Damage/FretteBodyPartTags.h"
#include "NativeGameplayTags.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_BodyPart_Head, "Frette.BodyPart.Head");
UE_DEFINE_GAMEPLAY_TAG(TAG_BodyPart_Torso, "Frette.BodyPart.Torso");
UE_DEFINE_GAMEPLAY_TAG(TAG_BodyPart_LeftArm, "Frette.BodyPart.LeftArm");
UE_DEFINE_GAMEPLAY_TAG(TAG_BodyPart_RightArm, "Frette.BodyPart.RightArm");
UE_DEFINE_GAMEPLAY_TAG(TAG_BodyPart_LeftHand, "Frette.BodyPart.LeftHand");
UE_DEFINE_GAMEPLAY_TAG(TAG_BodyPart_RightHand, "Frette.BodyPart.RightHand");
UE_DEFINE_GAMEPLAY_TAG(TAG_BodyPart_LeftLeg, "Frette.BodyPart.LeftLeg");
UE_DEFINE_GAMEPLAY_TAG(TAG_BodyPart_RightLeg, "Frette.BodyPart.RightLeg");
UE_DEFINE_GAMEPLAY_TAG(TAG_BodyPart_LeftFoot, "Frette.BodyPart.LeftFoot");
UE_DEFINE_GAMEPLAY_TAG(TAG_BodyPart_RightFoot, "Frette.BodyPart.RightFoot");

TMap<FGameplayTag, TSubclassOf<UFretteBodyPartAttributeSet>> Frette::Damage::TagToAttributeSetMap;

