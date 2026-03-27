#include "Components/FretteTemperatureZoneComponent.h"
#include "Components/FretteTemperatureComponent.h"
#include "Components/ShapeComponent.h"
#include "GameFramework/Character.h"
#include "PhysicsEngine/PhysicsAsset.h"

UFretteTemperatureZoneComponent::UFretteTemperatureZoneComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UFretteTemperatureZoneComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!GetOwner()->HasAuthority())
	{
		SetComponentTickEnabled(false);
		return;
	}

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

		UFretteTemperatureComponent* TempComp =
			Character->FindComponentByClass<UFretteTemperatureComponent>();

		if (!TempComp)
			continue;

		TArray<FName> CurrentBones =
			GetAffectedBonesFromSkelMesh(Character->GetMesh());

		TSet<FName>& ActiveBones = ActiveBonesPerCharacter.FindOrAdd(Character).Bones;

		for (const FName& Bone : CurrentBones)
		{
			if (!ActiveBones.Contains(Bone))
			{
				ActiveBones.Add(Bone);
				TempComp->AddBodyPartTemperatureModifier(TemperatureChange, Bone);
			}
		}

		for (auto It = ActiveBones.CreateIterator(); It; ++It)
		{
			if (!CurrentBones.Contains(*It))
			{
				TempComp->AddBodyPartTemperatureModifier(-TemperatureChange, *It);
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

		const FVector WorldPos = Body->GetUnrealWorldTransform().GetLocation();
		const FVector LocalPos = ZoneTransform.InverseTransformPositionNoScale(WorldPos);

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

	if (OverlappingCharacters.Num() == 1)
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

	if (FCharacterActiveBones* Bones = ActiveBonesPerCharacter.Find(Character))
	{
		if (UFretteTemperatureComponent* TemperatureComponent =
			Character->FindComponentByClass<UFretteTemperatureComponent>())
		{
			for (const FName& Bone : Bones->Bones)
			{
				TemperatureComponent->AddBodyPartTemperatureModifier(-TemperatureChange, Bone);
			}
		}

		ActiveBonesPerCharacter.Remove(Character);
	}

	if (OverlappingCharacters.Num() == 0)
	{
		SetComponentTickEnabled(false);
	}
}