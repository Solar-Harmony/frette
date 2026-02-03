#include "Components/FallDamageComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Character/FretteBaseCharacter.h"

//Permet de créer un tag dans la liste des tags de l'éditeur et de le référencer par TAG_Effect_Movement_FallDamage
UE_DEFINE_GAMEPLAY_TAG(TAG_Effect_Movement_FallDamage, "Effect.Movement.FallDamage");

void UFallDamageComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<AFretteBaseCharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("FallDamageComponent requires an ACharacter owner."));
		return;
	}

	MovementComponent = OwnerCharacter->GetCharacterMovement();
	if (!MovementComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("FallDamageComponent requires CharacterMovementComponent."));
		return;
	}

	BindMovementEvents();
}

void UFallDamageComponent::BindMovementEvents()
{
	OwnerCharacter->MovementModeChangedDelegate.AddDynamic(this, &UFallDamageComponent::MovementModeChanged);
	OwnerCharacter->LandedDelegate.AddDynamic(this, &UFallDamageComponent::Landed);
	OwnerCharacter->OnReachedJumpApex.AddDynamic(this, &UFallDamageComponent::OnJumpApexReached);
}

void UFallDamageComponent::MovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	if (MovementComponent->MovementMode == MOVE_Falling)
	{
		//Le notifyApex se fait set a false chaque foit que le notifyApex est triggered
		//donc il faut le re set a true au début des jumps
		MovementComponent->bNotifyApex = true;
		FallStartHeight = OwnerCharacter->GetActorLocation().Z;
	}
}

void UFallDamageComponent::Landed(const FHitResult& Hit)
{
	ApplyFallDamage(Hit.ImpactPoint.Z - FallStartHeight);
}

//Override le FallStartHeight a l'apex du saut 
void UFallDamageComponent::OnJumpApexReached()
{
	FallStartHeight = OwnerCharacter->GetActorLocation().Z;
}

void UFallDamageComponent::ApplyFallDamage(float DistanceFell) const
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter);

	if (DistanceFell <= -600.f)
	{
		if (Config == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("FallDamageComponent missing Config Data Asset"));
			return;
		}

		FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		const FGameplayEffectSpecHandle NewHandle = ASC->MakeOutgoingSpec(Config->FallDamageEffect, 1, EffectContext);

		const float DamageAmount = Config->DamageCurve.GetRichCurveConst()->Eval(FMath::Abs(DistanceFell));
		NewHandle.Data->SetSetByCallerMagnitude(TAG_Effect_Movement_FallDamage, -DamageAmount);

		if (NewHandle.IsValid())
		{
			//TODO: Les dégats vont devoir être appliqué aux jambes plutot qu'au character en général
			ASC->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), ASC);
		}

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Took Fall Damage: ") + FString::SanitizeFloat(DamageAmount));
	}
}