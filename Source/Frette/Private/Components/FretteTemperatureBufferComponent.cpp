#include "Components/FretteTemperatureBufferComponent.h"
#include "Components/FretteTemperatureComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/BodyPart/FretteBodyPartComponent.h"
#include "Components/BodyPart/FretteBodyPartData.h"
#include "GameFramework/Character.h"
#include "PhysicsEngine/BodySetup.h"

//Pas sur de l'autorité / multiplayer
UFretteTemperatureBufferComponent::UFretteTemperatureBufferComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = true;
	UniqueId = FGuid::NewGuid();
}

void UFretteTemperatureBufferComponent::BeginPlay()
{
	Super::BeginPlay();

	SetComponentTickInterval(1.f);

	OverlapShape = GetOwner()->FindComponentByClass<UShapeComponent>();

	if (!ensure(OverlapShape))
		return;

	OverlapShape->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
	OverlapShape->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnOverlapEnd);

	SetComponentTickEnabled(false);
}

void UFretteTemperatureBufferComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character)
		return;
	
	UFretteTemperatureComponent* TempComp =
		Character->FindComponentByClass<UFretteTemperatureComponent>();
		
	TempComp->AddBuffer(this, ThermalImpedance);
}

void UFretteTemperatureBufferComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character)
		return;

	UFretteTemperatureComponent* TempComp =
		Character->FindComponentByClass<UFretteTemperatureComponent>();
		
	TempComp->ClearBuffer(this);
}