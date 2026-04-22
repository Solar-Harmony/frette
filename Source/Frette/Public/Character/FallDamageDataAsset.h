#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FallDamageDataAsset.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FFallDamageThreshold
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMax = "0.0"))
	float FallHeight = -600.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0"))
	float DamageValue = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (Categories = "Frette.BodyPart"))
	TArray<FGameplayTag> AffectedBones;
};

UCLASS()
class UFallDamageDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	
	#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);

		DamageThresholds.Sort([](const FFallDamageThreshold& A, const FFallDamageThreshold& B) {
			return A.FallHeight > B.FallHeight;
		});
	}
	#endif

	UPROPERTY(EditDefaultsOnly)
	TArray<FFallDamageThreshold> DamageThresholds;
};