#pragma once

#include "CoreMinimal.h"
#include "FretteTemperatureBufferComponent.h"
#include "GameplayTagContainer.h"
#include "BodyPart/FretteBodyPartTags.h"
#include "Components/ActorComponent.h"
#include "Core/FretteWorldSettings.h"
#include "FretteTemperatureComponent.generated.h"

class UFretteBodyPartComponent;

USTRUCT(BlueprintType)
struct FTemperatureContribution
{
	GENERATED_BODY()

	FTemperatureContribution() = default;

	FTemperatureContribution(float Temperature, float Flow)
		: Temperature(Temperature)
		, Flow(Flow) {}

	UPROPERTY()
	float Temperature;

	UPROPERTY()
	float Flow;
};

USTRUCT(BlueprintType)
struct FTemperatureKey
{
	GENERATED_BODY()

	FTemperatureKey() = default;

	FTemperatureKey(FGameplayTag BodyPart, FGuid SourceId)
		: BodyPart(BodyPart)
		, SourceId(SourceId) {}

	UPROPERTY()
	FGameplayTag BodyPart;

	UPROPERTY()
	FGuid SourceId;

	bool operator==(const FTemperatureKey& Other) const
	{
		return BodyPart == Other.BodyPart && SourceId == Other.SourceId;
	}
};

FORCEINLINE uint32 GetTypeHash(const FTemperatureKey& Key)
{
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(Key.BodyPart));
	Hash = HashCombine(Hash, GetTypeHash(Key.SourceId));
	return Hash;
}

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FRETTE_API UFretteTemperatureComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float TimeBetweenTemperatureChange = 0.5f;

	UFUNCTION(BlueprintCallable)
	void AddToAmbientTemperature(float NewAmbientTemperature);

	UFUNCTION(BlueprintCallable)
	void AddBodyPartTemperatureContribution(FTemperatureContribution Contribution, FGameplayTag BodyPartTag, FGuid SourceId);
	void AddBodyPartTemperatureContribution(FTemperatureContribution Contribution, FName BoneName, FGuid SourceId);
	void ClearBodyPartTemperatureContribution(FGameplayTag BodyPartTag, FGuid SourceId);
	void ClearBodyPartTemperatureContribution(FName BoneName, FGuid SourceId);
	void ClearBodyPartTemperatureContributions(FGuid SourceId);
	
	void AddBuffer(UFretteTemperatureBufferComponent* buffer, float ThermalImpedance);
	void ClearBuffer(UFretteTemperatureBufferComponent* buffer);

protected:
	UFretteTemperatureComponent();
	void OnRegister();
	virtual void BeginPlay() override;
	void OnTemperatureTick();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//TODO: Temporaire le temps qu'on ai un vrai systeme de calcul de température ambiante selon l'environnement autour du joueur
	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	float AmbientTemperature = -30;

	UPROPERTY(EditDefaultsOnly, Category="Frette|Temperature", meta=(ClampMin="0.0", Units="Celsius",
		ToolTip="Temperature difference below which damping reduces the applied temperature change. This prevents oscillations and ensures smooth convergence toward the target temperature."))
	float DampingThreshold = 5;

	UPROPERTY(EditDefaultsOnly, Category="Frette|Temperature", meta=(ForceUnits="°C/s", ClampMin="0.0",
		ToolTip="Diffusion strength between neighboring body parts. Each degree of temperature difference generates this much temperature change per second (scaled by thermal impedance)."))
	float BoneTagNeighboursDiffusionFlow = 0.2;

	UPROPERTY(EditDefaultsOnly, Category="Frette|Temperature", meta=(ForceUnits="°C/s", ClampMin="0.0",
		ToolTip="Base rate at which temperature moves toward ambient when no other effects dominate. This rate increases with the temperature difference (scaled and clamped) and is reduced by thermal impedance."))
	float AmbientFlow = 0.1;

	const FGameplayTag TemperatureEffectTag = TAG_BodyPartValues_Temperature;
	const FGameplayTag ThermalImpedanceEffectTag = TAG_BodyPartValues_ThermalImpedance;

	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UFretteBodyPartComponent> BodyPartComponent;

	UPROPERTY()
	TMap<FTemperatureKey, FTemperatureContribution> BodyPartTemperatureContributions;

	UPROPERTY()
	TObjectPtr<AFretteWorldSettings> WorldSettings;

	UPROPERTY()
	TMap<FGameplayTag, FGameplayTagContainer> BoneTagNeighbours;

	FTimerHandle TemperatureTickHandle;
	
	TMap<UFretteTemperatureBufferComponent*, float> BufferThermalImpedances;
};