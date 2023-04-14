// Andrii Kmet 

#include "SupportActor.h"

#include "ProceduralRoom.h"


// Sets default values
ASupportActor::ASupportActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASupportActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASupportActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ASupportActor::GetItemSize(AActor* Item)
{
	if (!Item) { return FVector::ZeroVector; }
	FBox ItemBox = Item->GetComponentsBoundingBox();
	if (!ItemBox.IsValid) { return FVector::ZeroVector; }

	return ItemBox.GetSize();
}

float ASupportActor::GetItemRadius(AActor* Item) {
	if (!Item) { return NULL; }
	FBox ItemBBox = Item->GetComponentsBoundingBox();
	if (!ItemBBox.IsValid) { return NULL; }

	FVector MinPoint = ItemBBox.Min;
	FVector MaxPoint = ItemBBox.Max;
	MinPoint.Z = MaxPoint.Z = 0.f;
	return FVector::Dist(MinPoint, MaxPoint) / 2;
}

FVector ASupportActor::GetRandomPointInSquare(const FVector2D & Upper, const FVector2D& Lower, const float& FloorHeight)
{
	float XAxis = FMath::RandRange(Lower.X, Upper.X);
	float YAxis = FMath::RandRange(Lower.Y, Upper.Y);
	return FVector(XAxis, YAxis, FloorHeight);
}

void ASupportActor::FloorRandomRotation(AActor* Floor, FVector FloorSize)
{
	if (!Floor) { return; }

	FVector FloorLocation = Floor->GetActorLocation();
	FRotator FloorRotation = Floor->GetActorRotation();

	int32 RandAngle = FMath::RandRange(-3, 3);
	float Angle = 90.f * RandAngle;

	if (Angle == 90.f || Angle == -270.f) {
		FloorRotation.Yaw += Angle;
		Floor->SetActorRotation(FloorRotation);
		FloorLocation.X += FloorSize.X;
	}
	else if (Angle == 180.f || Angle == -180.f) {
		FloorRotation.Yaw += Angle;
		Floor->SetActorRotation(FloorRotation);
		FloorLocation.X += FloorSize.X;
		FloorLocation.Y += FloorSize.Y;
	}
	else if (Angle == 270.f || Angle == -90.f) {
		FloorRotation.Yaw += Angle;
		Floor->SetActorRotation(FloorRotation);
		FloorLocation.Y += FloorSize.Y;
	}
	else { return; }

	Floor->SetActorLocationAndRotation(FloorLocation, FloorRotation);
}

TArray<FVector> ASupportActor::Linspace(FVector Start, FVector End, int32 dens)  //FVector start, FVector end, int32 sizeOf
{
	TArray<FVector> vectors;
	if (dens <= 0) {
		return vectors;
	}
	else {
		FVector nextVector;
		for (int32 i = 0; i <= dens + 1; i++) {
			float Alpha = (float)i / (float)dens;
			nextVector.X = FMath::Lerp(Start.X, End.X, Alpha);
			nextVector.Y = FMath::Lerp(Start.Y, End.Y, Alpha);
			nextVector.Z = FMath::Lerp(Start.Z, End.Z, Alpha);
			vectors.Add(nextVector);
		}
	}
	return vectors;
}

FVector ASupportActor::Rand_LRUD_Step()
{
	TArray<FVector> StepDirection = { FVector::LeftVector, FVector::RightVector, FVector::BackwardVector, FVector::ForwardVector };
	int32 RandStep = FMath::RandRange(0, StepDirection.Num() - 1);

	return StepDirection[RandStep];
}

bool ASupportActor::IsVectorInsideArea(const FVector& Vector, const FVector& TopLeftPoint, const FVector& BottomRightPoint)
{
	return Vector.X > TopLeftPoint.X && Vector.X < BottomRightPoint.X &&
		Vector.Y < TopLeftPoint.Y && Vector.Y > BottomRightPoint.Y;
}

int32 ASupportActor::SnapToGrid(float Point, float GridDivision)
{
	int32 RoundedValue = FMath::RoundToInt(Point);
	int32 SnapValue = FMath::RoundToInt(RoundedValue / GridDivision) * GridDivision;
	if (FMath::Abs(SnapValue - RoundedValue) > FMath::Abs(GridDivision / 2)) {
		SnapValue += (GridDivision > 0) ? GridDivision : -GridDivision;
	}
	return SnapValue;
}

bool ASupportActor::IsSquareSideBiggerThanCheckValue(const TPair<FVector2D, FVector2D>& Square, float CheckValue)
{
	float SideX = FMath::Abs(Square.Value.X - Square.Key.X);
	float SideY = FMath::Abs(Square.Value.Y - Square.Key.Y);

	bool CheckXSize = SideX > CheckValue;
	bool CheckYSize = SideY > CheckValue;

	if(CheckXSize && CheckYSize) {
		return true;
	}
	return false;
}

void ASupportActor::SortByDistance(TArray<FVector>& Points, const FVector& CheckPoint)
{
	Points.Sort([&CheckPoint](const FVector& A, const FVector& B) {
		return (A - CheckPoint).SizeSquared() < (B - CheckPoint).SizeSquared();
	});
}

bool ASupportActor::DistanceValidation(TArray<FVector>& SpawnedItems, FVector CheckPoint, float Distance)
{
	SortByDistance(SpawnedItems, CheckPoint);
	for (int32 j = 0; j < SpawnedItems.Num(); ++j) {
		if (FVector::PointsAreNear(SpawnedItems[j], CheckPoint, Distance)) {
			return false;
		}
	}
	return true;
}