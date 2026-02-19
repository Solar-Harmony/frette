#include "Character/FrettePlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Equipments/FretteFakeEquipmentComponent.h"
#include "GameplayAbilitySystem/FretteAbilitySystemComponent.h"
#include "GameplayAbilitySystem/FretteAttributeSet.h"
#include "Player/FrettePlayerState.h"

class AFrettePlayerState;

AFrettePlayerCharacter::AFrettePlayerCharacter()
{
	// Create the first person mesh that will be viewed only by this character's owner
	FPMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FPMesh->SetupAttachment(GetMesh());
	FPMesh->SetOnlyOwnerSee(true);
	FPMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FPMesh->SetCollisionProfileName(FName("NoCollision"));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->bUsePawnControlRotation = true;
	Camera->bEnableFirstPersonFieldOfView = true;
	Camera->bEnableFirstPersonScale = true;
	Camera->FirstPersonFieldOfView = 70.0f;
	Camera->FirstPersonScale = 0.6f;

	InventoryComponent = CreateDefaultSubobject<UFakeInventoryComponent>(TEXT("Inventory"));
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
	const FVector MoveAxis3D = FVector(MoveAxis.X, MoveAxis.Y, 0.f);
	const FVector MovementDirWS = Camera->GetComponentRotation().RotateVector(-MoveAxis3D);
	AddMovementInput(MovementDirWS * 10000.0f, 1.0f);
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

void AFrettePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Fixes weird rotation at the beginning of the game
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
}