#include "Components/FretteTemperatureZoneComponent.h"
#include "Components/FretteTemperatureComponent.h"
#include "Components/ShapeComponent.h"
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

	SetComponentTickInterval(1.f);

	OverlapShape = GetOwner()->FindComponentByClass<UShapeComponent>();

	if (!ensure(OverlapShape))
		return;

	OverlapShape->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
	OverlapShape->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnOverlapEnd);

	SetComponentTickEnabled(false);
}

void UFretteTemperatureZoneComponent::TemperatureZoneTick()
{
	for (ACharacter* Character : OverlappingCharacters)
	{
		if (!Character)
			continue;

		UFretteTemperatureComponent* TemperatureComponent =
			Character->FindComponentByClass<UFretteTemperatureComponent>();

		if (!TemperatureComponent)
			continue;

		TArray<FName> BonesCurrentlyInZone =
			GetAffectedBonesFromSkelMesh(Character->GetMesh());

		TSet<FName>& ActiveBones = ActiveBonesPerCharacter.FindOrAdd(Character).Bones;

		for (const FName& Bone : BonesCurrentlyInZone)
		{
			if (!ActiveBones.Contains(Bone))
			{
				ActiveBones.Add(Bone);
				TemperatureComponent->AddBodyPartTemperatureFlow(TemperatureChange, Bone, UniqueId);
			}
		}

		for (auto It = ActiveBones.CreateIterator(); It; ++It)
		{
			if (!BonesCurrentlyInZone.Contains(*It))
			{
				TemperatureComponent->ClearBodyPartTemperatureFlow(*It, UniqueId);
				It.RemoveCurrent();
			}
		}
	}
}

void UFretteTemperatureZoneComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TemperatureZoneTick();
}

TArray<FName> UFretteTemperatureZoneComponent::GetAffectedBonesFromSkelMesh(
	const USkeletalMeshComponent* SkelMesh) const
{
	TArray<FName> AffectedBones;

	if (!SkelMesh)
		return AffectedBones;

	//Le plus simple que j'ai trouvé vus que je ne réussi pas a get les bones directement avec l'event d'overlap
	const FTransform ZoneTransform = OverlapShape->GetComponentTransform();
	const FVector ZoneExtent = OverlapShape->GetCollisionShape().GetExtent();

	for (const FBodyInstance* Body : SkelMesh->Bodies)
	{
		if (!Body || !Body->IsValidBodyInstance())
			continue;

		UBodySetup* BodySetup = Body->GetBodySetup();
		if (!BodySetup)
			continue;

		//Met la position du bone en relation avec la zone
		const FVector WorldPos = Body->GetUnrealWorldTransform().GetLocation();
		const FVector LocalPos = ZoneTransform.InverseTransformPositionNoScale(WorldPos);

		//Comme l'étendu de la zone est la même des deux coté plutot que vérifier si la position est aussi >= -ZoneExtent 
		//je vérifie juste si la valeur absolue de la position est inférieur a l'étendu
		if (FMath::Abs(LocalPos.X) <= ZoneExtent.X &&
			FMath::Abs(LocalPos.Y) <= ZoneExtent.Y &&
			FMath::Abs(LocalPos.Z) <= ZoneExtent.Z)
		{
			AffectedBones.AddUnique(BodySetup->BoneName);
		}
	}

	return AffectedBones;
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
		TemperatureComponent->ClearBodyPartTemperatureFlows(UniqueId);
	}

	if (OverlappingCharacters.IsEmpty())
	{
		SetComponentTickEnabled(false);
	}
}