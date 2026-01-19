#include "FrettePlayerCharacter.h"

#include "Components/CapsuleComponent.h"
#include "AbilitySystemComponent.h"
#include "FretteAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"

AFrettePlayerCharacter::AFrettePlayerCharacter()
{
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetCapsuleComponent());
	
	// Create the ability system component
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	// Create the attribute set
	AttributeSet = CreateDefaultSubobject<UFretteAttributeSet>(TEXT("AttributeSet"));
	
	// Create health widget component for showing health to other players
	HealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
	HealthWidgetComponent->SetupAttachment(GetMesh());
	HealthWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	HealthWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthWidgetComponent->SetDrawSize(FVector2D(150.f, 30.f));
	
	// Enable ticking for stamina management
	PrimaryActorTick.bCanEverTick = true;
}

void AFrettePlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	// Initialize ability system on server
	if (AbilitySystemComponent)
	{
		InitializeAbilitySystem();
	}
}

void AFrettePlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	// Initialize ability system on client
	if (AbilitySystemComponent)
	{
		InitializeAbilitySystem();
	}
}

UAbilitySystemComponent* AFrettePlayerCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AFrettePlayerCharacter::InitializeAbilitySystem()
{
	if (AbilitySystemComponent && AttributeSet)
	{
		AbilitySystemComponent->InitStats(UFretteAttributeSet::StaticClass(), nullptr);
	}
}

void AFrettePlayerCharacter::DoPlayerMove(FVector2D MoveAxis)
{
	const FVector MoveAxis3D = FVector(MoveAxis.X, MoveAxis.Y, 0.f);
	const FVector MovementDirWS = Camera->GetComponentRotation().RotateVector(-MoveAxis3D);
	
	// Apply sprint multiplier if sprinting and has stamina
	float SpeedMultiplier = 1.0f;
	if (bIsSprinting && GetStamina() > 0.f)
	{
		SpeedMultiplier = SprintSpeedMultiplier;
	}
	
	AddMovementInput(MovementDirWS * 10000.0f * SpeedMultiplier, 1.0f);
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

void AFrettePlayerCharacter::DoPlayerSprint(bool bInIsSprinting)
{
	bIsSprinting = bInIsSprinting;
}

void AFrettePlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Handle stamina drain and regen
	if (HasAuthority())
	{
		HandleStaminaDrain(DeltaTime);
	}
}

void AFrettePlayerCharacter::HandleStaminaDrain(float DeltaTime)
{
	if (!AbilitySystemComponent || !AttributeSet)
	{
		return;
	}
	
	float CurrentStamina = AttributeSet->GetStamina();
	float MaxStamina = AttributeSet->GetMaxStamina();
	
	// Drain stamina while sprinting and moving
	if (bIsSprinting && GetVelocity().Size() > 0.f)
	{
		float NewStamina = FMath::Max(0.f, CurrentStamina - StaminaDrainRate * DeltaTime);
		AttributeSet->SetStamina(NewStamina);
	}
	// Regenerate stamina when not sprinting
	else if (CurrentStamina < MaxStamina)
	{
		float NewStamina = FMath::Min(MaxStamina, CurrentStamina + StaminaRegenRate * DeltaTime);
		AttributeSet->SetStamina(NewStamina);
	}
}

void AFrettePlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	
	if (!HasAuthority() || !AttributeSet)
	{
		return;
	}
	
	// Calculate fall damage based on velocity
	float FallSpeed = -GetVelocity().Z;
	
	if (FallSpeed > FallDamageThreshold)
	{
		float Damage = (FallSpeed - FallDamageThreshold) * FallDamageMultiplier;
		float CurrentHealth = AttributeSet->GetHealth();
		float NewHealth = FMath::Max(0.f, CurrentHealth - Damage);
		AttributeSet->SetHealth(NewHealth);
		
		UE_LOG(LogTemp, Warning, TEXT("Fall damage: %f (speed: %f)"), Damage, FallSpeed);
	}
}

float AFrettePlayerCharacter::GetHealth() const
{
	return AttributeSet ? AttributeSet->GetHealth() : 0.f;
}

float AFrettePlayerCharacter::GetMaxHealth() const
{
	return AttributeSet ? AttributeSet->GetMaxHealth() : 0.f;
}

float AFrettePlayerCharacter::GetStamina() const
{
	return AttributeSet ? AttributeSet->GetStamina() : 0.f;
}

float AFrettePlayerCharacter::GetMaxStamina() const
{
	return AttributeSet ? AttributeSet->GetMaxStamina() : 0.f;
}

float AFrettePlayerCharacter::GetHealthPercent() const
{
	float MaxHealthValue = GetMaxHealth();
	return MaxHealthValue > 0.f ? GetHealth() / MaxHealthValue : 0.f;
}

float AFrettePlayerCharacter::GetStaminaPercent() const
{
	float MaxStaminaValue = GetMaxStamina();
	return MaxStaminaValue > 0.f ? GetStamina() / MaxStaminaValue : 0.f;
}

