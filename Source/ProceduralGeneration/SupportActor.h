// Andrii Kmet 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SupportActor.generated.h"

UCLASS()
class PROCEDURALGENERATION_API ASupportActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASupportActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	FVector GetItemSize(AActor* Item);

	float GetItemRadius(AActor* Item);

	FVector GetRandomPointInSquare(const FVector2D& Upper, const FVector2D& Lower, const float& FloorHeight);

	void FloorRandomRotation(AActor* Floor, FVector FloorSize);

	TArray<FVector> Linspace(FVector Start, FVector End, int32 dens);

	FVector Rand_LRUD_Step();

	bool IsVectorInsideArea(const FVector& Vector, const FVector& TopLeftPoint, const FVector& BottomRightPoint);

	int32 SnapToGrid(float Point, float GridDivision);

	bool IsSquareSideBiggerThanCheckValue(const TPair<FVector2D, FVector2D>& Square, float CheckValue);

	void SortByDistance(TArray<FVector>& Points, const FVector& CheckPoint);

	bool DistanceValidation(TArray<FVector>& SpawnedItems, FVector CheckPoint, float Distance);
};
