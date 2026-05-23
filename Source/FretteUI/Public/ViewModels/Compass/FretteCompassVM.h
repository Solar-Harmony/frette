#pragma once

#include "CoreMinimal.h"
#include "FretteViewModel.h"
#include "MVVMViewModelBase.h"
#include "Character/Player/Components/FretteCompassComponent.h"
#include "Core/FretteGameplayStatics.h"
#include "FretteCompassVM.generated.h"

#define LOCTEXT_NAMESPACE "Frette"

UCLASS()
class FRETTEUI_API UFretteCompassVM : public UFretteViewModel
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, FieldNotify)
	FText GetPrettyDirection() const
	{
		switch (CardinalDirection)
		{
			case ECardinalDirection::North:
				return LOCTEXT("DirN", "\x2191 North");
			case ECardinalDirection::NorthEast:
				return LOCTEXT("DirNE", "\x2196 North-east");
			case ECardinalDirection::East:
				return LOCTEXT("DirE", "\x2190 East");
			case ECardinalDirection::SouthEast:
				return LOCTEXT("DirSE", "\x2199 South-east");
			case ECardinalDirection::South:
				return LOCTEXT("DirS", "\x2193 South");
			case ECardinalDirection::SouthWest:
				return LOCTEXT("DirSW", "\x2198 South-west");
			case ECardinalDirection::West:
				return LOCTEXT("DirW", "\x2192 West");
			case ECardinalDirection::NorthWest:
				return LOCTEXT("DirNW", "\x2197 North-west");
			default:
				checkNoEntry();
				return INVTEXT("");
		}
	}
	
protected:
	virtual void Bind() override
	{
		PlayerCharacter->CompassComponent->OnUpdateDirection.BindUObject(this, &UFretteCompassVM::OnUpdateDirection);
	}
	
	void OnUpdateDirection(ECardinalDirection InCardinalDirection)
	{
		if (UE_MVVM_SET_PROPERTY_VALUE(CardinalDirection, InCardinalDirection))
		{
			UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetPrettyDirection);
		}
	}
	
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	ECardinalDirection CardinalDirection;
};

#undef LOCTEXT_NAMESPACE
