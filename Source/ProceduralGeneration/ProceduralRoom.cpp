// Andrii Kmet 

#include "ProceduralRoom.h"
#include "SupportActor.h"

#include "FloorNode.h"
#include "Floor.h"

#include "Components/BoxComponent.h"
#include "Components/SceneCaptureComponentCube.h"

// TODO spatial partitioning algorithms for distance spawn


// Basic functions
AProceduralRoom::AProceduralRoom()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Procedural box volume
	RootComponent = CreateDefaultSubobject<USceneCaptureComponent>(TEXT("ProceduralBoxVolume"));
	ProceduralBoxVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(ProceduralBoxVolume);
}

void AProceduralRoom::BeginPlay()
{
	Super::BeginPlay();

	if(bProceduralActivation) 
	{
		SupportClass = NewObject<ASupportActor>(this);

		// Basic constructors
		ItemArrayValidation();
		SetProceduralVolumeScale();
		GetInsetPoints();

		// Walls/floor generation
		GenerateFloor();
		GenerateWalls();

		// Interior generation
		if (DistanceSpawnActivation) { DistSpawn(); }

		if (GridSpawnActivation) { GridSpawn(); }

		if (FloorSplitSpawnActivation) { FloorSplitSpawn(); }
	}
}

void AProceduralRoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UE_LOG(LogTemp, Warning, TEXT("Floor: %d, Wall: %d"), static_cast<int32>(FloorSize.X), static_cast<int32>(WallSize.X));
	//DrawDebugLine(GetWorld(), BoxTopLeft, BoxTopLeft + 500 * FVector::ZAxisVector, FColor::Red, true, -1, 0, 4);
	//DrawDebugLine(GetWorld(), BoxBottomRight, BoxBottomRight + 500 * FVector::ZAxisVector, FColor::Blue, true, -1, 0, 4);
	//DrawDebugLine(GetWorld(), InsetTopLeft, InsetTopLeft + 700 * FVector::ZAxisVector, FColor::Red, true, -1, 0, 4);
	//DrawDebugLine(GetWorld(), InsetBottomRight, InsetBottomRight + 700 * FVector::ZAxisVector, FColor::Blue, true, -1, 0, 4);
}

// Basic constructors
void AProceduralRoom::ItemArrayValidation()
{
	FloorsArray.Remove(nullptr);
	WallsArray.Remove(nullptr);
	InteriorArray.Remove(nullptr);
	CornerArray.Remove(nullptr);
}

void AProceduralRoom::SetProceduralVolumeScale()
{
	if (WallsArray.IsEmpty() || FloorsArray.IsEmpty() || CornerArray.IsEmpty()) { return; }
	bBoxComplete = false;

	AActor* WallRef = GetWorld()->SpawnActor<AActor>(WallsArray[0], GetActorLocation(), FRotator::ZeroRotator);
	AActor* FloorRef = GetWorld()->SpawnActor<AActor>(FloorsArray[0], GetActorLocation(), FRotator::ZeroRotator);
	AActor* CornerRef = GetWorld()->SpawnActor<AActor>(CornerArray[0], GetActorLocation(), FRotator::ZeroRotator);

	FloorSize = SupportClass->GetItemSize(FloorRef); FloorRef->Destroy();
	WallSize = SupportClass->GetItemSize(WallRef); WallRef->Destroy();
	CornerSize = SupportClass->GetItemSize(CornerRef); CornerRef->Destroy();

	int32 Accuracy = 2;
	if (FMath::Abs(static_cast<int32>(FloorSize.X) - static_cast<int32>(WallSize.X)) > Accuracy) { return; }
	bBoxComplete = true;

	GridVolumeSize.X = FMath::RandRange(MinRoomSize, MaxRoomSize);
	GridVolumeSize.Y = FMath::RandRange(MinRoomSize, MaxRoomSize);

	MaxRoomHeight = FMath::RandRange(1, MaxRoomHeight);

	float XRandSize = (GridVolumeSize.X * FloorSize.X) * 0.5f;
	float YRandSize = (GridVolumeSize.Y * FloorSize.Y) * 0.5f;
	float ZSize = (WallSize.Z * MaxRoomHeight) * 0.5f;

	ProceduralBoxVolume->SetBoxExtent(FVector(XRandSize, YRandSize, ZSize));
	FVector Location = GetActorLocation();
	Location.Z += ZSize;
	SetActorLocation(Location);
}

void AProceduralRoom::GetInsetPoints()
{
	FBox FloorBox = GetComponentsBoundingBox();

	FloorCenter = FloorBox.GetCenter();
	BoxVolumeSize = FloorBox.GetSize();

	BoxTopLeft = FloorBox.Min;  
	BoxBottomRight = FloorBox.Max;
	BoxBottomRight.Z = BoxTopLeft.Z;
	
	FloorHeight = FloorSize.Z;
	FloorCenter.Z = FloorSize.Z;

	InsetTopLeft = FVector(BoxTopLeft.X + InsetThickness, BoxTopLeft.Y + InsetThickness, FloorHeight);
	InsetBottomRight = FVector(BoxBottomRight.X - InsetThickness, BoxBottomRight.Y - InsetThickness, FloorHeight);
}

// Support functions

void AProceduralRoom::DrawDebugSquare(const FVector2D& TopLeft, const FVector2D& BottomRight, const float& Height, FColor Colore)
{
	FColor Color = Colore;//FColor::MakeRandomColor();
	int32 DepthPriority = 0;
	int32 Thickness = 6;
	int32 LifeTime = -1;

	DrawDebugLine(GetWorld(), FVector(TopLeft.X, TopLeft.Y, Height), FVector(BottomRight.X, TopLeft.Y, Height), Color, true, LifeTime, DepthPriority, Thickness);
	DrawDebugLine(GetWorld(), FVector(BottomRight.X, TopLeft.Y, Height), FVector(BottomRight.X, BottomRight.Y, Height), Color, true, LifeTime, DepthPriority, Thickness);
	DrawDebugLine(GetWorld(), FVector(BottomRight.X, BottomRight.Y, Height), FVector(TopLeft.X, BottomRight.Y, Height), Color, true, LifeTime, DepthPriority, Thickness);
	DrawDebugLine(GetWorld(), FVector(TopLeft.X, BottomRight.Y, Height), FVector(TopLeft.X, TopLeft.Y, Height), Color, true, LifeTime, DepthPriority, Thickness);
}

// Spawn functions

AActor* AProceduralRoom::SpawnItemFromArr(TArray<TSubclassOf<AActor>> ItemArr, const FVector& Location, const FRotator& Rotation)
{
	if (ItemArr.IsEmpty()) { return nullptr; }

	int32 RandPosition = FMath::RandRange(0, ItemArr.Num() - 1);
	AActor* SpawnedItem = GetWorld()->SpawnActor<AActor>(ItemArr[RandPosition], Location, Rotation);
	return SpawnedItem;
}

void AProceduralRoom::RandPositionSpawnArr(UClass* Item, int32 quantity, FVector ItemStartLocation, bool RandRotation)
{
	if (!Item || quantity == 0) { return; }

	FVector CurrentLocation = ItemStartLocation;
	TArray<FVector> SpawnedObjectsLocation;

	for (int32 i = 0; i < quantity; ++i) {
		FRotator ItemRotation = FRotator::ZeroRotator;
		if (RandRotation) {
			ItemRotation = FRotator(0.f, FMath::RandRange(0.f, 360.f), 0.f);
		}

		if(i > 0) {
			FVector NewLocation = FVector::ZeroVector;
			FVector StepDirection = SupportClass->Rand_LRUD_Step();

			float ObjectAxisSize = 100.f;
			NewLocation = CurrentLocation + StepDirection * ObjectAxisSize;

			if(SpawnedObjectsLocation.Contains(NewLocation)) {
				NewLocation = CurrentLocation + (- StepDirection * ObjectAxisSize);
			}
			CurrentLocation = NewLocation;
		}
		AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(Item, CurrentLocation, ItemRotation);

		if (SupportClass->IsVectorInsideArea(CurrentLocation, InsetTopLeft, InsetBottomRight)) {
			SpawnedActor->Destroy();
		}

		SpawnedObjectsLocation.Add(CurrentLocation);
	}
}

AActor* AProceduralRoom::SpawnSwitchMethod(TArray<TSubclassOf<AActor>> ItemArr, int32 ItemPosition, const FVector& Location, const FRotator& Rotation)
{
	if (ItemArr.IsEmpty()) { return nullptr; }

	AActor* Item;
	if (bRandomizedItem) {
		Item = SpawnItemFromArr(ItemArr, Location, Rotation);
	}
	else {
		Item = GetWorld()->SpawnActor<AActor>(ItemArr[ItemPosition], Location, Rotation);
	}
	return Item;
}

AActor* AProceduralRoom::SpawnRandomItemInSquare(TArray<TSubclassOf<AActor>> ItemArr, const FVector2D& TopLeft, const FVector2D& BottomRight)
{
	if (ItemArr.IsEmpty()) { return nullptr; }

	FVector RandLocation = SupportClass->GetRandomPointInSquare(TopLeft, BottomRight, FloorHeight);
	FRotator RandRotation = FRotator(0.f, FMath::RandRange(0.f, 360.f), 0.f);

	AActor* SpawnedItem = SpawnItemFromArr(ItemArr, RandLocation, RandRotation);
	return SpawnedItem;
}

// Item generators

void AProceduralRoom::GenerateFloor()
{
	if (FloorsArray.IsEmpty() || !bBoxComplete) { return; }

	int32 RandFloor = FMath::RandRange(0, FloorsArray.Num() - 1);
	for (int32 i = 0; i < GridVolumeSize.X; ++i) {
		for (int32 j = 0; j < GridVolumeSize.Y; ++j) {
			FVector CellCenter(BoxTopLeft.X + (FloorSize.X * i), BoxTopLeft.Y + (FloorSize.Y * j), BoxTopLeft.Z);

			AActor* FloorCell = SpawnSwitchMethod(FloorsArray, RandFloor, CellCenter, FRotator::ZeroRotator);
			SupportClass->FloorRandomRotation(FloorCell, FloorSize);

			FVector CellMinLocation = FloorCell->GetComponentsBoundingBox().Min;
			if (CellMinLocation.Z < BoxTopLeft.Z) {
				FVector CellLocation = FloorCell->GetActorLocation();
				CellLocation.Z += FloorSize.Z;
				FloorCell->SetActorLocation(CellLocation);
			}
		}
	}
}

void AProceduralRoom::GenerateWalls()
{
	if (WallsArray.IsEmpty() || CornerArray.IsEmpty() || !bBoxComplete) { return; }

	const TArray<FVector> PositionMap = { FVector::YAxisVector, -FVector::XAxisVector, -FVector::YAxisVector,FVector::XAxisVector};
	const TArray<FIntVector2> CornerMap = { FIntVector2(1, 1), FIntVector2(-1, 1), FIntVector2(-1, -1),FIntVector2(1, -1) };

	FVector CurrentPoint((BoxTopLeft.X + BoxVolumeSize.X) - CornerSize.X, BoxTopLeft.Y, FloorHeight);
	FRotator CornerRotation = FRotator::ZeroRotator;

	int32 RandCorner = FMath::RandRange(0, CornerArray.Num() - 1);
	int32 RandWall = FMath::RandRange(0, WallsArray.Num() - 1);

	for (int32 i = 0; i < PositionMap.Num(); ++i) {
		FVector HeightLevel = CurrentPoint;
		// Spawn corners
		for (int32 h = 0; h < MaxRoomHeight; ++h) {
			SpawnSwitchMethod(CornerArray, RandCorner, HeightLevel, CornerRotation);
			HeightLevel.Z += CornerSize.Z;
		}

		CornerRotation.Yaw += 90.f;
		CurrentPoint.X += CornerSize.X * CornerMap[i].X;
		CurrentPoint.Y += CornerSize.Y * CornerMap[i].Y;

		int WallsIter; 
		if(PositionMap[i].GetAbs() == FVector::XAxisVector) {
			WallsIter = GridVolumeSize.X; 
		}
		else {
			WallsIter = GridVolumeSize.Y; 
		}
		// Spawn walls
		for (int32 j = 0; j < WallsIter - 1; ++j) {
			HeightLevel = CurrentPoint;
			for (int32 h = 0; h < MaxRoomHeight; ++h) {
				//Place to spawn doorway
				SpawnSwitchMethod(WallsArray, RandWall, HeightLevel, CornerRotation);
				HeightLevel.Z += CornerSize.Z;
			}
			CurrentPoint += PositionMap[i] * WallSize.X;
		}
	}
}

// Interior generators

// Distance spawn

void AProceduralRoom::DrawDistanceSpawn(TArray<FVector>& SpawnedItems)
{
	for (auto& CurrentPoint : SpawnedItems) {
		SupportClass->SortByDistance(SpawnedItems,CurrentPoint);
		FColor Color = FColor::Green;
		if (FVector::PointsAreNear(CurrentPoint, SpawnedItems[0], DistBetwenItems)) {
			Color = FColor::Red;
		}
		DrawDebugLine(GetWorld(), CurrentPoint, SpawnedItems[0], Color, true, -1, 0, 4);
	}
}

void AProceduralRoom::DistSpawn()
{
	if (ObjectNumber <= 0 || !bBoxComplete) { return; }
	if (DistBetwenItems < 0.f) { DistBetwenItems = 0.f; }

	TArray<FVector> RandLocations;

	for (int32 i = 0; i < ObjectNumber; ++i) {
		FVector RandLocation = SupportClass->GetRandomPointInSquare(FVector2D(InsetTopLeft), FVector2D(InsetBottomRight), FloorHeight);
		FRotator RandRotation = FRotator(0.f, FMath::RandRange(0.f, 360.f), 0.f);

		if (RandLocations.Num() > 0) {
			int32 MemoryCheck = 0;
			while (!SupportClass->DistanceValidation(RandLocations, RandLocation, DistBetwenItems)) {
				if (MemoryCheck >= 10000) { break; }
				RandLocation = SupportClass->GetRandomPointInSquare(FVector2D(InsetTopLeft), FVector2D(InsetBottomRight), FloorHeight);
				++MemoryCheck;
			}
		}
		RandLocations.Add(RandLocation);

		int32 RandItem = FMath::RandRange(0, InteriorArray.Num() - 1);

		if(SpawnArrangeActivation) {
			RandPositionSpawnArr(InteriorArray[RandItem], SpawnArrange, RandLocation, false);
		}
		else {
			SpawnItemFromArr(InteriorArray, RandLocation, RandRotation);
		}
		if (bDrawDistanceSpawn) {
			DrawDistanceSpawn(RandLocations);
		}
	}
}

// Grid Spawn
void AProceduralRoom::DrawGridSpawn(TArray<FVector>& SpawnedItems, TArray<float>& Radius)
{
	FBox FloorBox = GetComponentsBoundingBox();
	float VerticalLenghts = FloorBox.GetSize().X - InsetThickness * 2;
	float HorizontalLenghts = FloorBox.GetSize().Y - InsetThickness * 2;

	for (int32 i = 0; i < VerticalPoints.Num() - 1; ++i) {
		FVector VerticalEndGridPoint = VerticalPoints[i] + HorizontalLenghts * -FVector::YAxisVector;
		DrawDebugLine(GetWorld(), VerticalPoints[i], VerticalEndGridPoint, FColor::Green, true, -1, 0, 4);
	}

	for (int32 i = 0; i < HorizontalPoints.Num() - 1; ++i) {
		FVector HorizontalEndGridPoint = HorizontalPoints[i] + VerticalLenghts * -FVector::XAxisVector;
		DrawDebugLine(GetWorld(), HorizontalPoints[i], HorizontalEndGridPoint, FColor::Green, true, -1, 0, 4);
	}

	for(int32 i = 0; i < SpawnedItems.Num() - 1; ++i) {
		DrawDebugCircle(GetWorld(), SpawnedItems[i], Radius[i], 48, FColor::Red, true, -1.f, 0, 2.5f, FVector::YAxisVector, FVector::XAxisVector, true);
	}
}

void AProceduralRoom::CreateLineGrid()
{
	if (XGridSize <= 0 || YGridSize <= 0 || !bBoxComplete) { return; }
	FBox FloorBox = GetComponentsBoundingBox();

	float VerticalLenghts = FloorBox.GetSize().X - InsetThickness * 2;
	float HorizontalLenghts = FloorBox.GetSize().Y - InsetThickness * 2;

	FVector StartPoint = InsetBottomRight;
	FVector EndVertical = StartPoint - VerticalLenghts * FVector::XAxisVector;
	FVector EndHorizontal = StartPoint - HorizontalLenghts * FVector::YAxisVector;

	VerticalPoints = SupportClass->Linspace(StartPoint, EndVertical, XGridSize);
	HorizontalPoints = SupportClass->Linspace(StartPoint, EndHorizontal, YGridSize);
}

void AProceduralRoom::GridSpawn()
{
	CreateLineGrid();
	if (VerticalPoints.IsEmpty() || HorizontalPoints.IsEmpty() || !bBoxComplete) { return; }

	TArray<FVector> SpawnedItems;
	TArray<float> RadiusArr;

	float Radius;
	for (int32 i = 0; i < VerticalPoints.Num() - 2; ++i) {
		for (int32 j = 0; j < HorizontalPoints.Num() - 2; ++j) {
			//InteriorArray[0].GetDefaultObject()->GetSimpleCollisionRadius();
			if (const auto SpawnedItem = SpawnItemFromArr(InteriorArray, FloorCenter, FRotator::ZeroRotator)) {
				Radius = SupportClass->GetItemRadius(SpawnedItem);
				FVector UpperLeft(VerticalPoints[i].X - Radius, HorizontalPoints[j].Y - Radius, FloorHeight);
				FVector LowerRight(VerticalPoints[i + 1].X + Radius, HorizontalPoints[j + 1].Y + Radius, FloorHeight);

				FVector RandLocation = SupportClass->GetRandomPointInSquare(FVector2D(UpperLeft), FVector2D(LowerRight), FloorHeight);
				FRotator RandRotation = FRotator(0.f, FMath::RandRange(0.f, 360.f), 0.f);

				SpawnedItems.Push(RandLocation);
				RadiusArr.Push(Radius);

				SpawnedItem->SetActorLocationAndRotation(RandLocation, RandRotation, false, nullptr, ETeleportType::None);
			}
		}

	}

	if(bDrawGridSpawn) {
		DrawGridSpawn(SpawnedItems, RadiusArr);
	}

}

// Room split spawn

void AProceduralRoom::FloorSplitSpawn()
{
	FVector2D TopLeft = FVector2D(InsetBottomRight.X, InsetTopLeft.Y);
	FVector2D BottomRight = FVector2D(InsetTopLeft.X, InsetBottomRight.Y);

	float SplitValidation = SplitValidationMetres * 100.f;
	float SnapFactor = SnapFactorMetres * 100.f;

	TUniquePtr<Floor> SplitFloorPtr = MakeUnique<Floor>(TopLeft, BottomRight, SplitValidation, SnapFactor, RandomizeSplitting, SplitChance);
	SplitFloorPtr->Partition();

	TArray<TSharedPtr<FloorNode>> FloorNodeStack = SplitFloorPtr->PartitionedFloor;

	for (int32 i = 0; i < FloorNodeStack.Num(); ++i) {
		FVector2D TopLeftD = FVector2D{ static_cast<float>(FloorNodeStack[i]->GetCornerCordinates().UpperLeftX), static_cast<float>(FloorNodeStack[i]->GetCornerCordinates().UpperLeftY) };
		FVector2D BottomRightD = FVector2D{ static_cast<float>(FloorNodeStack[i]->GetCornerCordinates().LoverRightX), static_cast<float>(FloorNodeStack[i]->GetCornerCordinates().LoverRightY) };

		SpawnRandomItemInSquare(InteriorArray, TopLeftD, BottomRightD);

		if (bDrawSplitSpawn) { 
			DrawDebugSquare(TopLeftD, BottomRightD, FloorHeight + 1.f, FColor::Green);
		}
	}
}
