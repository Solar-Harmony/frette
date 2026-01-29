#pragma once

#include "CoreMinimal.h"
#include "FretteBaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameplayAbilitySystem/DA_ClassLoadout.h"

#include "FrettePlayerCharacter.generated.h"

UCLASS()
class AFrettePlayerCharacter : public AFretteBaseCharacter
{
	GENERATED_BODY()

public:
	AFrettePlayerCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	UFUNCTION(BlueprintCallable)
	void DoPlayerMove(FVector2D MoveAxis);
	
	UFUNCTION(BlueprintCallable)
	void DoPlayerLook(FVector2D LookAxis);
	
	UFUNCTION(BlueprintCallable)
	void DoPlayerJump();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> Camera;
	
	UPROPERTY(EditDefaultsOnly, Category="Class")
	TObjectPtr<UDA_ClassLoadout> ClassLoadout;
		
private:
	virtual void InitAbilityActorInfo() override;
	void OnMaxSpeedChanged(const FOnAttributeChangeData& Data) const;
};
