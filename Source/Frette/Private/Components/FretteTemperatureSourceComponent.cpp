#include "Components/FretteTemperatureSourceComponent.h"

#include "AssetTypeCategories.h"
#include "BaseGizmos/GizmoElementArrowHead.h"
#include "Components/ArrowComponent.h"
#include "Components/DrawSphereComponent.h"
#include "Components/FretteTemperatureComponent.h"
#include "GameFramework/Character.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/BodyPart/FretteBodyPartComponent.h"
#include "Components/BodyPart/FretteBodyPartData.h"

// Sets default values for this component's properties
UFretteTemperatureSourceComponent::UFretteTemperatureSourceComponent()
{
	bUseAttachParentBound = true;

	PrimaryComponentTick.bCanEverTick = true;

	UniqueId = FGuid::NewGuid();

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(this);
	OverlapSphere->InitSphereRadius(DiffusionRadius);
	OverlapSphere->SetGenerateOverlapEvents(true);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionObjectType(ECC_WorldDynamic);
	OverlapSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapSphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

	#if WITH_EDITORONLY_DATA
	DebugText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("DebugText"));
	DebugText->SetVisibility(bShowNumbersAtSlice);
	const float TextSize = DiffusionRadius / 4.f;
	DebugText->SetRelativeLocation(FVector(0, 0, DiffusionRadius + 0.5 * TextSize));
	DebugText->SetWorldSize(TextSize);
	DebugText->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	DebugText->SetTextRenderColor(FColor::White);
	DebugText->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DebugText->SetHiddenInGame(true);
	DebugText->SetupAttachment(this);

	DebugSphereInner = CreateEditorOnlyDefaultSubobject<UDrawSphereComponent>(TEXT("Debug Sphere 2"));
	DebugSphereInner->SetupAttachment(this);
	DebugSphereInner->SetIsVisualizationComponent(true);
	DebugSphereInner->SetLineThickness(2.f);
	DebugSphereInner->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DebugSphereInner->SetHiddenInGame(true);
	DebugSphereInner->ShapeColor = FColor::Cyan;
	DebugSphereInner->InitSphereRadius(SourceRadius);

	FloorDiskMesh = CreateEditorOnlyDefaultSubobject<UStaticMeshComponent>(TEXT("FloorDiskMesh"));
	FloorDiskMesh->SetupAttachment(this);
	FloorDiskMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FloorDiskMesh->SetCastShadow(false);
	FloorDiskMesh->SetHiddenInGame(true);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Cylinder"));
	if (PlaneMesh.Succeeded())
	{
		FloorDiskMesh->SetStaticMesh(PlaneMesh.Object);
		const float Scale = RadialSlice * DiffusionRadius / 50.0f;
		FloorDiskMesh->SetRelativeScale3D(FVector(Scale, Scale, 0.01f));
	}

	SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeatSphere"));
	SphereMesh->SetupAttachment(this);
	SphereMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SphereMesh->SetHiddenInGame(true);
	SphereMesh->SetSimulatePhysics(false);
	SphereMesh->SetEnableGravity(false);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Engine/BasicShapes/Sphere"));
	if (SphereMeshAsset.Succeeded())
	{
		SphereMesh->SetStaticMesh(SphereMeshAsset.Object);
		const float Scale = DiffusionRadius / 50.f;
		SphereMesh->SetRelativeScale3D(FVector(Scale));

		static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset(TEXT("/Game/Actors/Effects/Area/FireArea/M_Heat.M_Heat"));
		if (MaterialAsset.Succeeded())
			HeatMaterial = MaterialAsset.Object;
	}
	#endif
}

void UFretteTemperatureSourceComponent::OnRegister()
{
	Super::OnRegister();

	if (UWorld* World = GetWorld())
		WorldSettings = Cast<AFretteWorldSettings>(World->GetWorldSettings());

	if (IsValid(HeatMaterial) && !IsValid(HeatMaterialInstance))
	{
		HeatMaterialInstance = UMaterialInstanceDynamic::Create(HeatMaterial, this);
		SphereMesh->SetMaterial(0, HeatMaterialInstance);
	}

	if (OverlapSphere)
		OverlapSphere->SetSphereRadius(DiffusionRadius);

	#if WITH_EDITORONLY_DATA
	UpdateDebug();
	#endif
}

void UFretteTemperatureSourceComponent::BeginPlay()
{
	Super::BeginPlay();

	SetComponentTickEnabled(false);
	
	SetComponentTickInterval(0.5f);

	OverlapSphere->OnComponentBeginOverlap.AddDynamic(
		this,
		&UFretteTemperatureSourceComponent::OnBeginOverlap);
	OverlapSphere->OnComponentEndOverlap.AddDynamic(
		this,
		&UFretteTemperatureSourceComponent::OnEndOverlap);
}

float UFretteTemperatureSourceComponent::ComputeTemperature(float r) const
{
	if (r <= SourceRadius)
		return SourceTemperature;
	if (r >= DiffusionRadius)
		return AmbientTemperature;

	float value = AmbientTemperature +
		(SourceTemperature - AmbientTemperature) *
		(SourceRadius / (DiffusionRadius - SourceRadius)) *
		((DiffusionRadius / r) - 1.f);

	return value;
}

float UFretteTemperatureSourceComponent::ComputeFlow(float r) const
{
	// MEMO Keep an eye on this if the numerical scheme is unstable
	constexpr float Epsilon = 1e-4;
	if (r <= SourceRadius)
	{
		// The flow is 0 inside the inner radius according to the math so we'll fake it so it feels consistent
		return ComputeFlow(SourceRadius + Epsilon);
	}
	if (r >= DiffusionRadius)
		return 0;

	// We will compute the spherically symmetric gradient to get the (flow) using centered finite difference
	const float Tprev = ComputeTemperature(r - Epsilon),
	            Tnext = ComputeTemperature(r + Epsilon);

	const float Gradient = (Tprev - Tnext) / (2 * Epsilon);

	return FlowStrength * Gradient;
}

// Called every frame
void UFretteTemperatureSourceComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (ACharacter* Character : OverlappingCharacters)
	{
		if (!Character)
			continue;

		UFretteTemperatureComponent* TempComp =
			Character->FindComponentByClass<UFretteTemperatureComponent>();

		UFretteBodyPartComponent* BodyPartComponent = Character->FindComponentByClass<UFretteBodyPartComponent>();
		USkeletalMeshComponent* SkeletalMeshComp = Character->FindComponentByClass<USkeletalMeshComponent>();

		if (!IsValid(TempComp) || !IsValid(BodyPartComponent) || !IsValid(SkeletalMeshComp))
			continue;

		const FVector SourcePos = OverlapSphere->GetComponentLocation();

		for (const TObjectPtr<UFretteBodyPartData>& Data : BodyPartComponent->BodyPartData)
		{
			if (Data->BodyPartTag.IsValid())
			{
				const FGameplayTag BodyPartTag = Data->BodyPartTag;
				FName BoneName;
				if (!BodyPartComponent->GetRepresentativeBoneForTag(BodyPartTag, BoneName)
					|| SkeletalMeshComp->GetBoneIndex(BoneName) == INDEX_NONE)
				{
					continue;
				}

				const FVector BonePos = SkeletalMeshComp->GetBoneLocation(BoneName, EBoneSpaces::WorldSpace);

				const float r = FVector::Dist(BonePos, SourcePos);

				// If the bone is outside the diffusion radius, we need to remove its contribution
				if (r > DiffusionRadius)
				{
					TempComp->ClearBodyPartTemperatureContribution(BodyPartTag, UniqueId);
					continue;
				}

				float Flow = ComputeFlow(r);
				float Temp = ComputeTemperature(r);

				// Since there can be objects between the heat source and the character,
				// we will perform a basic line of sight check if the bone is not in the
				// inner radius.
				if (r > SourceRadius)
				{
					FHitResult HitResult;
					FCollisionQueryParams QueryParams;
					QueryParams.AddIgnoredActor(GetOwner());
					QueryParams.AddIgnoredActor(Character);

					const FVector DirToBone = (BonePos - SourcePos).GetSafeNormal();
					FVector TraceEnd = SourcePos + (DirToBone * SourceRadius);

					bool bBlocked = GetWorld()->LineTraceSingleByChannel(
						HitResult,
						BonePos,
						TraceEnd,
						ECC_Visibility,
						QueryParams
						);

					if (bBlocked)
					{
						Flow *= ObstructionFactor;
						Temp = FMath::Lerp(AmbientTemperature, Temp, ObstructionFactor);
					}
				}

				TempComp->AddBodyPartTemperatureContribution(FTemperatureContribution(Temp, Flow), BodyPartTag, UniqueId);
			}
		}
	}
}

void UFretteTemperatureSourceComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character)
		return;

	OverlappingCharacters.Add(Character);

	if (!OverlappingCharacters.IsEmpty())
	{
		SetComponentTickEnabled(true);
	}
}

void UFretteTemperatureSourceComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character)
		return;

	OverlappingCharacters.Remove(Character);
	if (UFretteTemperatureComponent* TemperatureComponent =
		Character->FindComponentByClass<UFretteTemperatureComponent>())
	{
		TemperatureComponent->ClearBodyPartTemperatureContributions(UniqueId);
	}

	if (OverlappingCharacters.IsEmpty())
	{
		SetComponentTickEnabled(false);
	}
}

#if WITH_EDITORONLY_DATA
void UFretteTemperatureSourceComponent::UpdateDebugArrows()
{
	if (!IsValid(WorldSettings))
		return;

	const float r = RadialSlice * DiffusionRadius;
	const float TemperatureAtSlice = ComputeTemperature(r);
	const float FlowAtSlice = ComputeFlow(r);

	// The arrows are useful but it is important to have access to numbers for tuning
	FString Text = FString::Printf(TEXT("Temp: %.3f°C\nFlow: %.3f°C/s"),
		TemperatureAtSlice, FlowAtSlice);
	DebugText->SetText(FText::FromString(Text));

	if (ShowArrows == ETemperatureSourceArrowRole::None)
	{
		for (const auto Arrow : DebugArrows)
			Arrow->SetVisibility(false);
		return;
	}

	float Quantity = 0;
	if (ShowArrows == ETemperatureSourceArrowRole::Temperature)
		Quantity = ((TemperatureAtSlice - WorldSettings->MinTemperature)
			/ (WorldSettings->MaxTemperature - WorldSettings->MinTemperature)) * 200.f;
	else if (ShowArrows == ETemperatureSourceArrowRole::Flow)
		Quantity = FlowAtSlice * 20.f;

	for (int32 i = 0; i < NumberArrows; i++)
	{
		UArrowComponent* Arrow = nullptr;

		if (DebugArrows.Num() > i)
		{
			Arrow = DebugArrows[i];
		}
		else
		{
			FName ArrowName = FName(*FString::Printf(TEXT("Arrow_%d"), i));
			Arrow = NewObject<UArrowComponent>(this, UArrowComponent::StaticClass(), ArrowName);
			Arrow->SetupAttachment(this);
			Arrow->SetArrowSize(1.f);
			Arrow->SetHiddenInGame(true);
			Arrow->SetSimulatePhysics(false);
			Arrow->SetEnableGravity(false);
			Arrow->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Arrow->RegisterComponent();
			DebugArrows.Add(Arrow);
		}

		// This is a trick to have uniform arrows around the upper-sphere
		// If we just use an (i, j) angle grid approach for (azimuth, inclination),
		// the vectors clump to the top or the bottom
		const float t = (float)i / (float)NumberArrows;

		constexpr float GoldenAngle = 2.39996323f;
		const float Azimuth = i * GoldenAngle;
		const float z = 1.f - t;
		const float r_xy = FMath::Sqrt(1.f - z * z);

		FVector Dir;
		Dir.X = r_xy * FMath::Cos(Azimuth);
		Dir.Y = r_xy * FMath::Sin(Azimuth);
		Dir.Z = z;

		FVector ArrowPos = Dir * r;

		Arrow->SetRelativeLocation(ArrowPos);
		Arrow->SetRelativeRotation(FRotationMatrix::MakeFromX(Dir).ToQuat());
		Arrow->SetArrowLength(Quantity);
		if (Quantity == 0 || ShowArrows == ETemperatureSourceArrowRole::Temperature)
			Arrow->SetArrowColor(FColor::White);
		else if (Quantity > 0)
			Arrow->SetArrowColor(FColor::Red);
		else
			Arrow->SetArrowColor(FColor::Blue);

		Arrow->SetVisibility(true);
	}

	for (int32 i = NumberArrows; i < DebugArrows.Num(); i++)
	{
		if (DebugArrows[i])
		{
			DebugArrows[i]->SetVisibility(false);
		}
	}
}

void UFretteTemperatureSourceComponent::UpdateMaterial() const
{
	if (!IsValid(HeatMaterialInstance) || !IsValid(WorldSettings))
		return;

	HeatMaterialInstance->SetScalarParameterValue("InnerRadius", SourceRadius);
	HeatMaterialInstance->SetScalarParameterValue("OuterRadius", DiffusionRadius);
	HeatMaterialInstance->SetScalarParameterValue("SourceTemperature", SourceTemperature);
	HeatMaterialInstance->SetScalarParameterValue("AmbientTemperature", AmbientTemperature);
	HeatMaterialInstance->SetScalarParameterValue("MinTemperature", WorldSettings->MinTemperature);
	HeatMaterialInstance->SetScalarParameterValue("MaxTemperature", WorldSettings->MaxTemperature);
}

void UFretteTemperatureSourceComponent::UpdateDebug()
{
	DebugSphereInner->SetSphereRadius(SourceRadius);
	const float TextSize = DiffusionRadius / 4.f;
	DebugText->SetRelativeLocation(FVector(0, 0, DiffusionRadius + 0.5 * TextSize));
	DebugText->SetWorldSize(TextSize);
	DebugText->SetVisibility(bShowNumbersAtSlice);

	if (SphereMesh)
	{
		const float Scale = DiffusionRadius / 50.f;
		SphereMesh->SetRelativeScale3D(FVector(Scale));
	}
	
	if (FloorDiskMesh)
	{
		const float Scale = RadialSlice * DiffusionRadius / 50.0f;
		FloorDiskMesh->SetRelativeScale3D(FVector(Scale, Scale, 0.01f));
		FloorDiskMesh->SetVisibility(ShowArrows != ETemperatureSourceArrowRole::None || bShowNumbersAtSlice);
	}

	UpdateMaterial();
	UpdateDebugArrows();
}

void UFretteTemperatureSourceComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	OverlapSphere->SetSphereRadius(DiffusionRadius);
	UpdateDebug();
}
#endif