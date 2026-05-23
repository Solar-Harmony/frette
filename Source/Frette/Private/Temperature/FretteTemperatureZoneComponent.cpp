#include "Components/FretteTemperatureZoneComponent.h"
#include "Components/FretteTemperatureComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/BodyPart/FretteBodyPartComponent.h"
#include "Components/BodyPart/FretteBodyPartData.h"
#include "GameFramework/Character.h"
#include "PhysicsEngine/BodySetup.h"

//Pas sur de l'autorité / multiplayer
UFretteTemperatureZoneComponent::UFretteTemperatureZoneComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	UniqueId = FGuid::NewGuid();
}

void UFretteTemperatureZoneComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
		WorldSettings = Cast<AFretteWorldSettings>(World->GetWorldSettings());
	
	SetComponentTickInterval(WorldSettings->TimeBeforeTemperatureUpdates);

	OverlapShape = GetOwner()->FindComponentByClass<UShapeComponent>();

	if (!ensure(OverlapShape))
		return;

	OverlapShape->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
	OverlapShape->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnOverlapEnd);

	SetComponentTickEnabled(false);
}

void UFretteTemperatureZoneComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
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
				
				// If the bone is outside the mesh extents, we need to remove its contribution
				if (!OverlapShape->OverlapComponent(BonePos, FQuat::Identity, FCollisionShape::MakeSphere(1.f)))
				{
					TempComp->ClearBodyPartTemperatureContribution(BodyPartTag, UniqueId);
					continue;
				}
				
				TempComp->AddBodyPartTemperatureContribution(FTemperatureContribution(Temperature, Flow), BodyPartTag, UniqueId);
			}
		}
	}

}

void UFretteTemperatureZoneComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
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

void UFretteTemperatureZoneComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
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