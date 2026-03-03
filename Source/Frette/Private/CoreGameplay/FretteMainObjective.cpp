#include "CoreGameplay/FretteMainObjective.h"

AFretteMainObjective::AFretteMainObjective()
{
#if WITH_EDITORONLY_DATA
	DebugSphere = CreateDefaultSubobject<UDrawSphereComponent>(TEXT("DebugSphere"));
	DebugSphere->SetupAttachment(RootComponent);
	DebugSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DebugSphere->bIsEditorOnly = true;
	DebugSphere->SetHiddenInGame(true);
#endif
}

void AFretteMainObjective::BeginPlay()
{
	Super::BeginPlay();
	
#if WITH_EDITORONLY_DATA
	DebugSphere->SetSphereRadius(NearObjectiveRadiusCm);
#endif
}