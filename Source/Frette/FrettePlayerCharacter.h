#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"

#include "FrettePlayerCharacter.generated.h"

UCLASS()
class AFrettePlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFrettePlayerCharacter();
	
	UFUNCTION(BlueprintCallable)
	void DoPlayerMove(FVector2D MoveAxis);
	
	UFUNCTION(BlueprintCallable)
	void DoPlayerLook(FVector2D LookAxis);
	
	UFUNCTION(BlueprintCallable)
	void DoPlayerJump();
	
	UPROPERTY(BlueprintReadWrite)
	float Speed = 5.0f;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> Camera;
};
