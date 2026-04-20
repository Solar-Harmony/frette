#include "MyHUD.h"
#include "Engine/Canvas.h"

void AMyHUD::DrawHUD()
{
	Super::DrawHUD();
	#if WITH_EDITOR

	const FVector2D Center(Canvas->SizeX * 0.5f, Canvas->SizeY * 0.5f);
	const float Size = 10.f;

	DrawLine(Center.X - Size, Center.Y, Center.X + Size, Center.Y, FColor::Green, 1.f);
	DrawLine(Center.X, Center.Y - Size, Center.X, Center.Y + Size, FColor::Green, 1.f);
	#endif
}