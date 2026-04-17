#include "Components/FretteTemperatureSourceComponent.h"

#include "AssetTypeCategories.h"
#include "BaseGizmos/GizmoElementArrowHead.h"
#include "Components/ArrowComponent.h"
#include "Components/DrawSphereComponent.h"
#include "Components/FretteTemperatureComponent.h"
#include "GameFramework/Character.h"
#include "Components/InstancedStaticMeshComponent.h"

// Sets default values for this component's properties
UFretteTemperatureSourceComponent::UFretteTemperatureSourceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	UniqueId = FGuid::NewGuid();

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(this);
	OverlapSphere->InitSphereRadius(OuterRadius);
	OverlapSphere->SetGenerateOverlapEvents(true);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionObjectType(ECC_WorldDynamic);
	OverlapSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapSphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	OverlapSphere->OnComponentBeginOverlap.AddDynamic(
		this,
		&UFretteTemperatureSourceComponent::OnBeginOverlap);
	OverlapSphere->OnComponentEndOverlap.AddDynamic(
		this,
		&UFretteTemperatureSourceComponent::OnEndOverlap);

	#if WITH_EDITORONLY_DATA
	DebugSphereInner = CreateEditorOnlyDefaultSubobject<UDrawSphereComponent>(TEXT("Debug Sphere 2"));
	DebugSphereInner->SetupAttachment(this);
	DebugSphereInner->SetIsVisualizationComponent(true);
	DebugSphereInner->SetLineThickness(2.f);
	DebugSphereInner->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DebugSphereInner->SetMobility(EComponentMobility::Static);
	DebugSphereInner->SetHiddenInGame(true);
	DebugSphereInner->ShapeColor = FColor::Cyan;
	DebugSphereInner->InitSphereRadius(InnerRadius);

	SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeatSphere"));
	SphereMesh->SetupAttachment(this);
	SphereMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SphereMesh->SetHiddenInGame(true);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Engine/BasicShapes/Sphere"));
	if (SphereMeshAsset.Succeeded())
	{
		SphereMesh->SetStaticMesh(SphereMeshAsset.Object);
		float Scale = OuterRadius / 50.f;
		SphereMesh->SetRelativeScale3D(FVector(Scale));
		if (HeatMaterial)
		{
			HeatMaterialInstance = UMaterialInstanceDynamic::Create(HeatMaterial, this);
			SphereMesh->SetMaterial(0, HeatMaterialInstance);
			UpdateMaterial();
		}
	}

	UpdateDebugArrows();
	#endif
}

void UFretteTemperatureSourceComponent::UpdateDebugArrows()
{
	const float GoldenAngle = 2.39996323f; // helps avoid clumping
	const FVector Center = GetComponentLocation();
	const float r = VisualisationSlice * OuterRadius;
	
	float Flow;
	if (AroowsAreForTemperature)
		Flow = ((ComputeTemperature(r) - MinTemperature) / (MaxTemperature - MinTemperature)) * 200.f;
	else
		Flow = ComputeFlow(r) * 10.f;

	for (int32 i = 0; i < NumberFlowArrows; i++)
	{
		UArrowComponent* Arrow = nullptr;

		if (DebugDiffusionArrows.Num() > i)
		{
			Arrow = DebugDiffusionArrows[i];
		}
		else
		{
			FName ArrowName = FName(*FString::Printf(TEXT("Arrow_%d"), i));
			Arrow = NewObject<UArrowComponent>(this, UArrowComponent::StaticClass(), ArrowName);
			Arrow->SetupAttachment(this);
			Arrow->SetArrowSize(1.f);
			Arrow->SetHiddenInGame(true);
			DebugDiffusionArrows.Add(Arrow);
		}

		// This is a trick to have uniform arrows around the upper-sphere
		// If we just use a angle grid approach, the vectors clump to the top
		const float t = (float)i / (float)NumberFlowArrows;

		const float Azimuth = i * GoldenAngle;
		const float z = 1.0f - t;
		const float r_xy = FMath::Sqrt(1.0f - z * z);

		FVector Dir;
		Dir.X = r_xy * FMath::Cos(Azimuth);
		Dir.Y = r_xy * FMath::Sin(Azimuth);
		Dir.Z = z;

		FVector ArrowPos = Center + Dir * r;

		Arrow->SetWorldLocation(ArrowPos);
		Arrow->SetWorldRotation(FRotationMatrix::MakeFromX(Dir).ToQuat());
		Arrow->SetArrowLength(Flow);
		if (Flow == 0 || AroowsAreForTemperature)
			Arrow->SetArrowColor(FColor::White);
		else if (Flow > 0)
			Arrow->SetArrowColor(FColor::Red);
		else
			Arrow->SetArrowColor(FColor::Blue);

		Arrow->SetVisibility(true);
	}

	for (int32 i = NumberFlowArrows; i < DebugDiffusionArrows.Num(); i++)
	{
		if (DebugDiffusionArrows[i])
		{
			DebugDiffusionArrows[i]->SetVisibility(false);
		}
	}
}

void UFretteTemperatureSourceComponent::BeginPlay()
{
	Super::BeginPlay();
}

float UFretteTemperatureSourceComponent::ComputeFalloff(float r) const
{
	return (OuterRadius / r) - 1.0f;
}

float UFretteTemperatureSourceComponent::ComputeTemperature(float r) const
{
	if (r <= InnerRadius)
		return SourceTemperature;
	if (r >= OuterRadius)
		return AmbientTemperature;

	float value = AmbientTemperature +
		(SourceTemperature - AmbientTemperature) *
		(InnerRadius / (OuterRadius - InnerRadius)) *
		ComputeFalloff(r);

	return value;
}

float UFretteTemperatureSourceComponent::ComputeFlow(float r) const
{
	// MEMO Keep an eye on this if the numerical scheme is unstable
	constexpr float Epsilon = 1e-4;
	if (r <= InnerRadius)
	{
		// The flow is 0 inside the inner radius according to the math so we'll fake it so it feels consistent
		return ComputeFlow(InnerRadius + Epsilon);
	}
	if (r >= OuterRadius)
		return 0.f;

	// We will compute the spherically symmetric gradient to get the (flow) using centered finite difference
	const float Tprev = ComputeTemperature(r - Epsilon),
	            Tnext = ComputeTemperature(r + Epsilon);

	const float Gradient = (Tprev - Tnext) / (2 * Epsilon);

	return DiffusionStrength * Gradient;
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

		USkeletalMeshComponent* Mesh = Character->GetMesh();

		if (!TempComp || !Mesh)
			continue;

		const FVector SourcePos = OverlapSphere->GetComponentLocation();

		// Vu qu<on veut continuellement update 
		const int32 BoneCount = Mesh->GetNumBones();

		for (int32 i = 0; i < BoneCount; i++)
		{
			const FName BoneName = Mesh->GetBoneName(i);
			if (BoneName == NAME_None)
				continue;

			const FVector BonePos =
				Mesh->GetBoneLocation(BoneName, EBoneSpaces::WorldSpace);

			const float r = FVector::Dist(BonePos, SourcePos);

			// Evaluate field contribution directly
			const float Flow = ComputeFlow(r);
			const float Temp = ComputeTemperature(r);
			const float Falloff = ComputeTemperature(r);

			TempComp->AddBodyPartTemperatureContribution(FTemperatureContribution(Temp, Falloff, Flow), BoneName, UniqueId);
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
void UFretteTemperatureSourceComponent::UpdateMaterial()
{
	if (!HeatMaterialInstance)
		return;

	HeatMaterialInstance->SetScalarParameterValue("InnerRadius", InnerRadius);
	HeatMaterialInstance->SetScalarParameterValue("OuterRadius", OuterRadius);
	HeatMaterialInstance->SetScalarParameterValue("SourceTemperature", SourceTemperature);
	HeatMaterialInstance->SetScalarParameterValue("AmbientTemperature", AmbientTemperature);
	HeatMaterialInstance->SetScalarParameterValue("MinTemperature", -40);
	HeatMaterialInstance->SetScalarParameterValue("MaxTemperature", 1500);
}

void UFretteTemperatureSourceComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	OverlapSphere->SetSphereRadius(OuterRadius);
	DebugSphereInner->SetSphereRadius(InnerRadius);

	if (SphereMesh)
	{
		float Scale = OuterRadius / 50.f;
		SphereMesh->SetRelativeScale3D(FVector(Scale));
	}

	if (!HeatMaterialInstance && HeatMaterial)
	{
		HeatMaterialInstance = UMaterialInstanceDynamic::Create(HeatMaterial, this);
		SphereMesh->SetMaterial(0, HeatMaterialInstance);
	}

	UpdateMaterial();
	UpdateDebugArrows();
}
#endif