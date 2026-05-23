#include "Character/Player/FrettePlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "Character/Player/Components/FretteNotificationsComponent.h"
#include "Components/CapsuleComponent.h"
#include "Character/Player/Components/FretteCompassComponent.h"
#include "Temperature/FretteTemperatureComponent.h"
#include "Character/Components/BodyParts/FretteBodyPartComponent.h"
#include "Character/Equipment/FretteEquipmentComponent.h"
#include "Character/Components/AbilitySystem/FretteAbilitySystemComponent.h"
#include "Character/Components/AbilitySystem/FretteAttributeSet.h"
#include "Inventory/FretteInventoryComponent.h"
#include "Character/Player/FrettePlayerState.h"
#include "Core/FretteCollisionChannels.h"

class AFrettePlayerState;

AFrettePlayerCharacter::AFrettePlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SetReplicatingMovement(true);

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
	
	CompassComponent = CreateDefaultSubobject<UFretteCompassComponent>(TEXT("Compass Component"));
}

void AFrettePlayerCharacter::SetupPlayerCollisions() const
{
	GetMesh()->SetCollisionObjectType(ECC_CharacterMesh);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_CharacterMesh, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_CharacterMesh, ECR_Ignore);
}

void AFrettePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	USkeletalMeshComponent* MeshComponent = GetMesh();
	MeshComponent->bMultiBodyOverlap = true;
	MeshComponent->SetGenerateOverlapEvents(true);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetAllBodiesSimulatePhysics(false);
	MeshComponent->RecreatePhysicsState();

	SetupPlayerCollisions();

	UE_LOG(LogTemp, Warning, TEXT("Mesh RelativeRot: %s"), *GetMesh()->GetRelativeRotation().ToString());

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
	AddControllerYawInput(LookAxis.X * HorizontalLookSensitivity);
	AddControllerPitchInput(LookAxis.Y * VerticalLookSensitivity);
}

//Set la position de la caméra a la position du socket de tête du mesh (pour les animations)
//Et smooth la rotation pour avoir moins de jitter de petit movement de la souris
void AFrettePlayerCharacter::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	FTransform HeadTransform = GetMesh()->GetSocketTransform(FName("head"), RTS_World);
	FVector CameraLocation = HeadTransform.GetLocation();

	OutResult.Location = CameraLocation;
	
	if (bIsDead)
	{
		FQuat TargetQuat = HeadTransform.GetRotation();
		FQuat CurrentQuat = SmoothedControlRotation.Quaternion();
		
		// Use a much slower interpolation speed for the ragdoll transition (e.g. 5.0f instead of LookSmoothingSpeed which is 20.0f)
		// This creates a smooth blending effect where the camera gradually "falls" into alignment with the tumbling head
		CurrentQuat = FMath::QInterpTo(CurrentQuat, TargetQuat, DeltaTime, 5.0f);
		SmoothedControlRotation = CurrentQuat.Rotator();
		
		OutResult.Rotation = SmoothedControlRotation;
	}
	else
	{
		OutResult.Rotation = SmoothedControlRotation;
	}
	
	OutResult.FOV = Camera->FieldOfView;
}

void AFrettePlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bIsDead)
	{
		// Prevent head clipping visually by receding the mesh locally
		FVector DefaultRelativeLocation(0, 0, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		FVector CurrentRelLoc = GetMesh()->GetRelativeLocation();
		
		// Reconstruct the "clean" un-pushed head position
		FVector CurrentWorldOffset = GetActorTransform().TransformVectorNoScale(CurrentRelLoc - DefaultRelativeLocation);
		FVector CleanHeadLoc = GetMesh()->GetSocketLocation(FName("head")) - CurrentWorldOffset;
		
		FVector TraceStart = GetActorLocation();
		TraceStart.Z = CleanHeadLoc.Z;

		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(HeadClip), false, this);
		FHitResult Hit;
		
		float ProbeSize = 20.0f;
		FVector TargetRelativeLoc = DefaultRelativeLocation;
		
		if (GetWorld()->SweepSingleByChannel(
			Hit, 
			TraceStart, 
			CleanHeadLoc + (CleanHeadLoc - TraceStart).GetSafeNormal() * ProbeSize, 
			FQuat::Identity, 
			ECC_Camera, 
			FCollisionShape::MakeSphere(ProbeSize), 
			QueryParams))
		{
			// Only push back against mostly vertical walls to avoid mantling floor ledge snags
			if (FMath::Abs(Hit.Normal.Z) < 0.7f)
			{
				FVector PushBack = FVector::ZeroVector;
				if (Hit.bStartPenetrating)
				{
					PushBack = Hit.Normal * (Hit.PenetrationDepth + 1.0f);
				}
				else
				{
					PushBack = Hit.Location - CleanHeadLoc;
				}
				
				// Convert to local space to offset the mesh 
				FVector LocalPushBack = GetActorTransform().InverseTransformVectorNoScale(PushBack);
				
				// Only drag the mesh backward along its forward axis to avoid sideways sliding
				LocalPushBack.X = FMath::Min(0.0f, LocalPushBack.X);
				LocalPushBack.Y = 0.0f;
				LocalPushBack.Z = 0.0f; 
				TargetRelativeLoc += LocalPushBack;
			}
		}

		// Apply target instantly if we need to push back more, otherwise recover smoothly
		FVector CurrentRelLocClean = GetMesh()->GetRelativeLocation();
		FVector SmoothedLoc;
		if (TargetRelativeLoc.X < CurrentRelLocClean.X - 1.0f)
		{
			SmoothedLoc = TargetRelativeLoc; // Snap immediately to prevent clipping
		}
		else
		{
			SmoothedLoc = FMath::VInterpTo(CurrentRelLocClean, TargetRelativeLoc, DeltaSeconds, 15.0f);
		}
		
		SmoothedLoc.Z = DefaultRelativeLocation.Z; // Ensure Z remains properly locked
		GetMesh()->SetRelativeLocation(SmoothedLoc);

		FRotator TargetRotation = GetControlRotation();
		TargetRotation.Pitch = FRotator::NormalizeAxis(TargetRotation.Pitch);
		SmoothedControlRotation.Pitch = FRotator::NormalizeAxis(SmoothedControlRotation.Pitch);

		SmoothedControlRotation = FMath::RInterpTo(
			SmoothedControlRotation,
			TargetRotation,
			DeltaSeconds,
			LookSmoothingSpeed
			);
	}
}

//Je suis pas trop sur de ce qui devrait être appeler juste du coté serveur ou juste du coté client mais pour l'instant
//Ça semble fonctionner comme ça aumoin localement
//Les abilité ne sont pas donnés aux simulated proxies donc on peut pas vraiment tester (De ce que)
void AFrettePlayerCharacter::InitAbilityActorInfo()
{
	AFrettePlayerState* State = GetPlayerState<AFrettePlayerState>();
	check(State);
	check(State);
	AttributeSet = State->GetAttributeSet();
	AbilitySystemComponent = Cast<UFretteAbilitySystemComponent>(State->GetAbilitySystemComponent());
	AbilitySystemComponent->InitAbilityActorInfo(State, this);
	ApplyStartupEffects();
	SubToAttributeChanges();
}

void AFrettePlayerCharacter::Die()
{
	if (!HasAuthority())
		return;

	if (bIsDead)
		return;

	Super::Die();

	Multicast_HandleDeath(GetCharacterMovement()->Velocity);
	OnPlayerDied.Broadcast(this);
}

void AFrettePlayerCharacter::Multicast_HandleDeath_Implementation(FVector DeathVelocity)
{
	if (!HasAuthority())
	{
		Super::Die();
	}
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->SetMovementMode(MOVE_None);

	bUseControllerRotationYaw = false;

	HandleRagdoll(DeathVelocity);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->DisableInput(PlayerController);
	}
	
	
	if (HasAuthority())
	{
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, [this]() {
			Revive();
			BodyPartComponent->ResetValueForAllBodyParts(TAG_BodyPartValues_Health);
			BodyPartComponent->ResetValueForAllBodyParts(TAG_BodyPartValues_Temperature);
		}, ReviveTimer, false);
	}
}

void AFrettePlayerCharacter::HandleRagdoll(FVector DeathVelocity)
{
	GetMesh()->Stop();
	GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
	GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("pelvis"), true, true);
	GetMesh()->SetPhysicsLinearVelocity(DeathVelocity, false, FName("pelvis"));
}

void AFrettePlayerCharacter::Revive()
{
	Super::Revive(); 
	Multicast_HandleRevive();
}

void AFrettePlayerCharacter::Multicast_HandleRevive_Implementation()
{
	Super::Multicast_HandleRevive_Implementation();
	
	UnRagdoll();
}

void AFrettePlayerCharacter::UnRagdoll()
{
	USkeletalMeshComponent* PlayerMesh = GetMesh();

	const FVector RagdollLocation = PlayerMesh->GetBoneLocation(FName("pelvis"));

	SetActorLocation(RagdollLocation);

	PlayerMesh->SetAllBodiesSimulatePhysics(false);
	
	SetupPlayerCollisions();
	
	bUseControllerRotationYaw = true;
	
	PlayerMesh->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	
	PlayerMesh->SetRelativeLocationAndRotation(
		FVector(0, 0, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()),
		FRotator(0, -90.f, 0)
	);

	PlayerMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->EnableInput(PlayerController);
	}
}