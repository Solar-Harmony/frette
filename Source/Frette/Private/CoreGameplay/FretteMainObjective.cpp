#include "CoreGameplay/FretteMainObjective.h"

AFretteMainObjective::AFretteMainObjective()
{
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
#endif
}

void AFretteMainObjective::OnConstruction(const FTransform& Transform)
{
#if WITH_EDITORONLY_DATA
	DebugSphere->SetSphereRadius(NearObjectiveRadiusCm);
#endif
}