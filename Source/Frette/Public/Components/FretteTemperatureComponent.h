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
	//TODO: Devrais avoir une liste de modifiers qui ajoute ou réduise a la température
	//Donc le feu pourrais ajouter un tag feu qui donne 50 degres de chaleur
	// et on retire un modifier avec le tag feu quand on quitte le range du feu
	//Devras être setter par les éléments dans le monde
	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	int TargetTemperature = -20;

protected:
	UFretteTemperatureComponent();
	virtual void BeginPlay() override;
	void OnTemperatureTick();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY()
	FGameplayTag TemperatureEffectTag = TAG_BodyPartValues_Temperature;

	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UFretteBodyPartComponent> BodyPartComponent;

	FTimerHandle TemperatureTickHandle;
	int CurrentTemperature = 0;
};