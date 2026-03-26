#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "BodyPart/FretteBodyPartTags.h"
#include "Components/ActorComponent.h"
#include "FretteTemperatureComponent.generated.h"

class UFretteBodyPartComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FRETTE_API UFretteTemperatureComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float TimeBetweenTemperatureChange = 3.f;

	UFUNCTION(BlueprintCallable)
	void AddToAmbientTemperature(int NewAmbientTemperature);

	UFUNCTION(BlueprintCallable)
	void AddBodyPartTemperatureModifier(int NewTargetTemperature, FGameplayTag BodyPartTag);

protected:
	UFretteTemperatureComponent();
	virtual void BeginPlay() override;
	void OnTemperatureTick();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//TODO: Temporaire le temps qu'on ai un vrai systeme de calcul de température ambiante selon l'environnement autour du joueur
	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	int AmbientTemperature = -20;

	const FGameplayTag TemperatureEffectTag = TAG_BodyPartValues_Temperature;

	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UFretteBodyPartComponent> BodyPartComponent;

	UPROPERTY()
	TMap<FGameplayTag, int> BodyPartTemperatureTargets;

	FTimerHandle TemperatureTickHandle;
	int CurrentTemperature = 0;
};