#include "Components/FretteTemperatureBufferComponent.h"
#include "Components/FretteTemperatureComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Character.h"

UFretteTemperatureBufferComponent::UFretteTemperatureBufferComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFretteTemperatureBufferComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!ensure(Owner))
		return;
	
	if (UWorld* World = GetWorld())
		WorldSettings = Cast<AFretteWorldSettings>(World->GetWorldSettings());

	if (!ensure(WorldSettings))
		return;

	SetComponentTickInterval(WorldSettings->TimeBeforeTemperatureUpdates);

	if (!Owner->HasAuthority())
		return;

	if (!ensure(!BufferTagThermalImpedances.IsEmpty()))
	{
		UE_LOG(LogTemp, Warning, TEXT("UFretteTemperatureBufferComponent: No thermal buffer tags configured on %s"), *Owner->GetName());
		return;
	}

	ColliderThermalImpedances.Empty();
	BoundOverlapColliders.Empty();

	TArray<UPrimitiveComponent*> PrimitiveComponents;
	Owner->GetComponents(PrimitiveComponents);

	for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
	{
		if (!IsValid(PrimitiveComponent))
			continue;

		float ColliderImpedance = -1.0f;
		for (const auto& TagImpedancePair : BufferTagThermalImpedances)
		{
			if (!PrimitiveComponent->ComponentHasTag(TagImpedancePair.Key))
				continue;

			ColliderImpedance = FMath::Max(ColliderImpedance, FMath::Clamp(TagImpedancePair.Value, 0.0f, 1.0f));
		}

		if (ColliderImpedance < 0.0f)
			continue;

		ColliderThermalImpedances.Add(PrimitiveComponent, ColliderImpedance);
		BoundOverlapColliders.Add(PrimitiveComponent);
		
		PrimitiveComponent->SetGenerateOverlapEvents(true);
		PrimitiveComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		PrimitiveComponent->SetCollisionObjectType(ECC_WorldDynamic);
		PrimitiveComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		PrimitiveComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		PrimitiveComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
		
		PrimitiveComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
		PrimitiveComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnOverlapEnd);
	}

	if (!ensure(BoundOverlapColliders.IsEmpty()))
	{
		UE_LOG(LogTemp, Warning, TEXT("UFretteTemperatureBufferComponent: No primitive components matching configured thermal buffer tags found on %s"), *Owner->GetName());
	}
}

void UFretteTemperatureBufferComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
		return;

	if (!BoundOverlapColliders.Contains(OverlappedComp))
		return;

	UpdateCharacterBufferImpedance(Cast<ACharacter>(OtherActor));
}

void UFretteTemperatureBufferComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
		return;

	if (!BoundOverlapColliders.Contains(OverlappedComp))
		return;

	UpdateCharacterBufferImpedance(Cast<ACharacter>(OtherActor));
}

float UFretteTemperatureBufferComponent::GetCharacterMaxThermalImpedance(const ACharacter* Character) const
{
	if (!Character)
		return -1.0f;

	bool bHasAnyOverlap = false;
	float MaxThermalImpedance = 0.0f;
	for (const auto& Entry : ColliderThermalImpedances)
	{
		UPrimitiveComponent* Collider = Entry.Key.Get();
		if (!IsValid(Collider) || !BoundOverlapColliders.Contains(Collider))
		{
			continue;
		}

		if (Collider->IsOverlappingActor(Character))
		{
			bHasAnyOverlap = true;
			MaxThermalImpedance = FMath::Max(MaxThermalImpedance, Entry.Value);
		}
	}

	return bHasAnyOverlap ? MaxThermalImpedance : -1.0f;
}

void UFretteTemperatureBufferComponent::UpdateCharacterBufferImpedance(ACharacter* Character)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
		return;
	
	if (!Character)
		return;

	UFretteTemperatureComponent* TempComp = Character->FindComponentByClass<UFretteTemperatureComponent>();
	if (!TempComp)
		return;

	const float MaxThermalImpedance = GetCharacterMaxThermalImpedance(Character);
	if (MaxThermalImpedance < 0.0f)
	{
		TempComp->ClearBuffer(this);
		return;
	}

	TempComp->AddBuffer(this, MaxThermalImpedance);
}
