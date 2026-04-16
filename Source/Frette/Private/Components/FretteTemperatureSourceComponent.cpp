#include "Components/FretteTemperatureSourceComponent.h"

#include "BaseGizmos/GizmoElementArrowHead.h"
#include "Components/DrawSphereComponent.h"
#include "Evaluation/IMovieSceneEvaluationHook.h"

// Sets default values for this component's properties
UFretteTemperatureSourceComponent::UFretteTemperatureSourceComponent()
{

	PrimaryComponentTick.bCanEverTick = true;
	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
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
	DebugSphereInner->SetIsVisualizationComponent(true);
	DebugSphereInner->SetLineThickness(2.f);
	DebugSphereInner->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DebugSphereInner->SetMobility(EComponentMobility::Static);
	DebugSphereInner->SetHiddenInGame(true);
	DebugSphereInner->ShapeColor = FColor::Cyan;
	DebugSphereInner->InitSphereRadius(InnerRadius);

	SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeatSphere"));
	SphereMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SphereMesh->SetHiddenInGame(true);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Engine/BasicShapes/Sphere"));
	if (SphereMeshAsset.Succeeded())
	{
		SphereMesh->SetStaticMesh(SphereMeshAsset.Object);
		float Scale = OuterRadius / 50.f;
		SphereMesh->SetWorldScale3D(FVector(Scale));
	}

	if (HeatMaterial)
	{
		HeatMaterialInstance = UMaterialInstanceDynamic::Create(HeatMaterial, this);
		SphereMesh->SetMaterial(0, HeatMaterialInstance);
		UpdateMaterial();
	}
	#endif
}

void UFretteTemperatureSourceComponent::OnRegister()
{
	Super::OnRegister();

	if (AActor* Owner = GetOwner())
	{
		if (USceneComponent* Root = Owner->GetRootComponent())
		{
			#if WITH_EDITORONLY_DATA
			DebugSphereInner->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
			SphereMesh->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
			#endif
			OverlapSphere->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
}

void UFretteTemperatureSourceComponent::BeginPlay()
{
	Super::BeginPlay();
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
		((OuterRadius / r) - 1.0f);

	return value;
}

// Called every frame
void UFretteTemperatureSourceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UFretteTemperatureSourceComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Begin overlap: %s"), *GetNameSafe(OtherActor));
}

void UFretteTemperatureSourceComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("End overlap: %s"), *GetNameSafe(OtherActor));
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
		SphereMesh->SetWorldScale3D(FVector(Scale));
	}

	if (!HeatMaterialInstance && HeatMaterial)
	{
		HeatMaterialInstance = UMaterialInstanceDynamic::Create(HeatMaterial, this);
		SphereMesh->SetMaterial(0, HeatMaterialInstance);
	}

	UpdateMaterial();
}
#endif