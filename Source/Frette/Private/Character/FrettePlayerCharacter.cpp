#include "Character/FrettePlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "BodySetupCore.h"
#include "EnhancedInputComponent.h"
#include "Character/FretteNotificationsComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/FretteTemperatureComponent.h"
#include "Components/BodyPart/FretteBodyPartComponent.h"
#include "Equipment/FretteEquipmentComponent.h"
#include "GameplayAbilitySystem/FretteAbilitySystemComponent.h"
#include "GameplayAbilitySystem/FretteAttributeSet.h"
#include "Inventory/FretteInventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FrettePlayerState.h"

class AFrettePlayerState;

AFrettePlayerCharacter::AFrettePlayerCharacter()
{
	FPMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FPMesh->SetupAttachment(GetMesh());
	FPMesh->SetOnlyOwnerSee(true);
	FPMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FPMesh->SetCollisionProfileName(FName("NoCollision"));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(FPMesh, FName("head"));
	Camera->bUsePawnControlRotation = true;
	Camera->bEnableFirstPersonFieldOfView = true;
	Camera->bEnableFirstPersonScale = true;

	PlayerInventory = CreateDefaultSubobject<UFretteInventoryComponent>("Inventory Component");
	PlayerInventory->SetIsReplicated(true);

	EquipmentComponent = CreateDefaultSubobject<UFretteEquipmentComponent>(TEXT("Equipment Component"));
	EquipmentComponent->SetIsReplicated(true);

	NotificationsComponent = CreateDefaultSubobject<UFretteNotificationsComponent>(TEXT("Notifications Component"));
	NotificationsComponent->SetIsReplicated(true);

	BodyPartComponent = CreateDefaultSubobject<UFretteBodyPartComponent>(TEXT("BodyPartComponent"));
	BodyPartComponent->SetIsReplicated(true);

	BodyTemperatureComponent = CreateDefaultSubobject<UFretteTemperatureComponent>(TEXT("BodyTemperatureComponent"));
	BodyTemperatureComponent->SetIsReplicated(true);

}

//Client side
void AFrettePlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitAbilityActorInfo();
}

//Server side
void AFrettePlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();
}

void AFrettePlayerCharacter::DoPlayerMove(FVector2D MoveAxis)
{
	if (!Controller)
		return;

	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);

	const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(Forward, MoveAxis.X);
	AddMovementInput(Right, MoveAxis.Y);
}

void AFrettePlayerCharacter::DoPlayerLook(FVector2D LookAxis)
{
	AddControllerYawInput(LookAxis.X);
	AddControllerPitchInput(LookAxis.Y);
}

void AFrettePlayerCharacter::DoPlayerJump()
{
	Jump();
}

//Set la position de la caméra a la position du socket de tête du mesh (pour les animations)
//Et smooth la rotation pour avoir moins de jitter de petit movement de la souris
void AFrettePlayerCharacter::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	FRotator TargetRotation = GetControlRotation();
	TargetRotation.Pitch = FRotator::NormalizeAxis(TargetRotation.Pitch);

	SmoothedControlRotation.Pitch = FRotator::NormalizeAxis(SmoothedControlRotation.Pitch);

	SmoothedControlRotation = FMath::RInterpTo(
		SmoothedControlRotation,
		TargetRotation,
		DeltaTime,
		LookSmoothingSpeed
		);

	FTransform HeadTransform = GetMesh()->GetSocketTransform(FName("head"), RTS_World);

	OutResult.Location = HeadTransform.GetLocation();
	OutResult.Rotation = SmoothedControlRotation;
	OutResult.FOV = Camera->FieldOfView;
}

//Je suis pas trop sur de ce qui devrait être appeler juste du coté serveur ou juste du coté client mais pour l'instant
//Ça semble fonctionner comme ça aumoin localement
//Les abilité ne sont pas donnés aux simulated proxies donc on peut pas vraiment tester (De ce que)
void AFrettePlayerCharacter::InitAbilityActorInfo()
{
	AFrettePlayerState* State = GetPlayerState<AFrettePlayerState>();
	check(State);
	AttributeSet = State->GetAttributeSet();
	AbilitySystemComponent = Cast<UFretteAbilitySystemComponent>(State->GetAbilitySystemComponent());
	AbilitySystemComponent->InitAbilityActorInfo(State, this);
	ApplyStartupEffects();
	SubToAttributeChanges();
}

void AFrettePlayerCharacter::SetIsDead(bool bNewIsDead)
{
	if (!HasAuthority())
		return;

	if (bIsDead == bNewIsDead)
		return;

	bIsDead = bNewIsDead;

	if (bIsDead)
		Multicast_HandleDeath(GetCharacterMovement()->Velocity);

}

void AFrettePlayerCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AFrettePlayerCharacter::Multicast_HandleDeath_Implementation(FVector DeathVelocity)
{

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->SetMovementMode(MOVE_None);

	bUseControllerRotationYaw = false;

	GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	GetMesh()->Stop();
	GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
	GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("pelvis"), true, true);
	GetMesh()->SetPhysicsLinearVelocity(DeathVelocity, false, FName("pelvis"));

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
		PlayerController->DisableInput(PlayerController);
}

void AFrettePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Fixes weird rotation at the beginning of the game
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
}