#include "Character/FrettePlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "Player/FrettePlayerState.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayAbilitySystem/FretteAbilitySystemComponent.h"
#include "GameplayAbilitySystem/FretteAttributeSet.h"

class AFrettePlayerState;

AFrettePlayerCharacter::AFrettePlayerCharacter()
{
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetCapsuleComponent());
}

void AFrettePlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitAbilityActorInfo();
}

void AFrettePlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();
}

void AFrettePlayerCharacter::DoPlayerMove(FVector2D MoveAxis)
{
	const FVector MoveAxis3D = FVector(MoveAxis.X, MoveAxis.Y, 0.f);
	const FVector MovementDirWS = Camera->GetComponentRotation().RotateVector(-MoveAxis3D);
	AddMovementInput(MovementDirWS * 10000.0f, 1.0f);
}

void AFrettePlayerCharacter::DoPlayerLook(FVector2D LookAxis)
{
	const float YawDelta   = LookAxis.X;
	const float PitchDelta = LookAxis.Y;

	FRotator CameraRotation = Camera->GetComponentRotation();
	CameraRotation.Yaw += YawDelta;
	CameraRotation.Pitch = FMath::Clamp(CameraRotation.Pitch + PitchDelta, -89.f, 89.f);
	CameraRotation.Roll = 0.0f;
	
	Camera->SetWorldRotation(CameraRotation.Quaternion());
	
	CameraRotation.Yaw -= 90.0f;
	CameraRotation.Pitch = 0.0f;
	GetMesh()->SetWorldRotation(CameraRotation.Quaternion());
}

void AFrettePlayerCharacter::DoPlayerJump()
{
	Jump();
}

void AFrettePlayerCharacter::InitAbilityActorInfo()
{
	
	AFrettePlayerState* playerState = GetPlayerState<AFrettePlayerState>();
	check(playerState);
	AttributeSet =  playerState->GetAttributeSet();
	AbilitySystemComponent = Cast<UFretteAbilitySystemComponent>(playerState->GetAbilitySystemComponent());
	AbilitySystemComponent->InitAbilityActorInfo(playerState,this);
	AbilitySystemComponent->GrantAbilitiesFromLoadout(ClassLoadout);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UFretteAttributeSet::GetMaxSpeedAttribute())
		.AddUObject(this, &AFrettePlayerCharacter::OnMaxSpeedChanged);
	
	Cast<UFretteAbilitySystemComponent>(playerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	
	Super::InitAbilityActorInfo();
}

void AFrettePlayerCharacter::OnMaxSpeedChanged(const FOnAttributeChangeData& Data) const
{
	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,FString::Printf(TEXT("Max Speed changed to %f"),Data.NewValue));
	const float NewMaxSpeed = Data.NewValue;
    
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->MaxWalkSpeed = NewMaxSpeed;
	}
}

