#include "Components/FretteGameplayStatics.h"

ECardinalDirection UFretteGameplayStatics::DirVectorToCardinal(const FVector2D& Dir)
{
	check(!Dir.IsNearlyZero());

	const float Angle = FMath::Atan2(Dir.Y, Dir.X); // get vector angle between -180 and 180 deg
	constexpr float SectorSize = PI / 4; // divide circle in 8 sectors, 45 deg each
	const int Sector = FMath::RoundToInt(Angle / SectorSize); // round to nearest sector -> [-4, 4]
	const int SectorIdx = (Sector % 8 + 8) % 8; // remap to [0, 7]

	return static_cast<ECardinalDirection>(SectorIdx);
}

