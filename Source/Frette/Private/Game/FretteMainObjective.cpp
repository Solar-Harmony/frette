#include "Game/Treasure/FretteMainObjective.h"

#include "Net/UnrealNetwork.h"

AFretteMainObjective::AFretteMainObjective()
{
	bReplicates = true;
	bAlwaysRelevant = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionProfileName(TEXT("BlockAll"));
	Mesh->SetEnableGravity(false);
	Mesh->SetMobility(EComponentMobility::Static);
	SetRootComponent(Mesh);
	
#if WITH_EDITORONLY_DATA
	DebugSphere = CreateEditorOnlyDefaultSubobject<UDrawSphereComponent>(TEXT("Debug Sphere"));
	DebugSphere->SetIsVisualizationComponent(true);
	DebugSphere->SetLineThickness(10.f);
	DebugSphere->SetupAttachment(Mesh);
	DebugSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DebugSphere->SetMobility(EComponentMobility::Static);
	DebugSphere->SetHiddenInGame(true);
	
	DebugSphereInner = CreateEditorOnlyDefaultSubobject<UDrawSphereComponent>(TEXT("Debug Sphere 2"));
	DebugSphereInner->SetIsVisualizationComponent(true);
	DebugSphereInner->SetLineThickness(10.f);
	DebugSphereInner->SetupAttachment(Mesh);
	DebugSphereInner->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DebugSphereInner->SetMobility(EComponentMobility::Static);
	DebugSphereInner->SetHiddenInGame(true);
	DebugSphereInner->ShapeColor = FColor::Cyan;
#endif
}

void AFretteMainObjective::SetCollected(bool bInCollected)
{
	if (!HasAuthority())
		return;

	if (bIsCollected == bInCollected)
		return;

	bIsCollected = bInCollected;
	ApplyCollectedState();
}

void AFretteMainObjective::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFretteMainObjective, bIsCollected);
}

void AFretteMainObjective::OnRep_IsCollected()
{
	ApplyCollectedState();
}

void AFretteMainObjective::ApplyCollectedState()
{
	SetActorHiddenInGame(bIsCollected);
	SetActorEnableCollision(!bIsCollected);
	if (IsValid(Mesh))
	{
		Mesh->SetHiddenInGame(bIsCollected);
		Mesh->SetCollisionEnabled(bIsCollected ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);
	}
}

void AFretteMainObjective::OnConstruction(const FTransform& Transform)
{
#if WITH_EDITORONLY_DATA
	DebugSphere->SetSphereRadius(NearObjectiveRadiusCm);
	DebugSphereInner->SetSphereRadius(RightOnObjectiveRadiusCm);
#endif
}