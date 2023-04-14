// Andrii Kmet 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralRoom.generated.h"

class ASupportActor;

UCLASS()
class PROCEDURALGENERATION_API AProceduralRoom : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProceduralRoom();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	
	// Volume parameters

	FVector BoxVolumeSize;

	FIntVector2 GridVolumeSize;

	FVector FloorCenter;

	FVector BoxTopLeft;

	FVector BoxBottomRight;

	bool bBoxComplete;

	// Spawn parameters

	FVector FloorSize;

	FVector WallSize;

	FVector CornerSize;

	FVector InsetTopLeft;

	FVector InsetBottomRight;

	float FloorHeight;

	TArray<FVector> VerticalPoints;

	TArray<FVector> HorizontalPoints;

private:

	UPROPERTY()
	ASupportActor* SupportClass;

private:
	// Spawn attribute
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn attribute", meta = (AllowPrivateAccess = "true"))
		bool bProceduralActivation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn attribute", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* ProceduralBoxVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn attribute", meta = (AllowPrivateAccess = "true"))
		bool bRandomizedItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn attribute", meta = (AllowPrivateAccess = "true"))
		int32 MinRoomSize = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn attribute", meta = (AllowPrivateAccess = "true"))
		int32 MaxRoomSize = 12;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn attribute", meta = (AllowPrivateAccess = "true"))
		int32 MaxRoomHeight = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn attribute", meta = (AllowPrivateAccess = "true"))
		float InsetThickness;

	// Spawn objects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn objects", meta = (AllowPrivateAccess = "true"))
		TArray<TSubclassOf<AActor>> InteriorArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn objects", meta = (AllowPrivateAccess = "true"))
		TArray<TSubclassOf<AActor>> FloorsArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn objects", meta = (AllowPrivateAccess = "true"))
		TArray<TSubclassOf<AActor>> WallsArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn objects", meta = (AllowPrivateAccess = "true"))
		TArray<TSubclassOf<AActor>> CornerArray;

	// Distance spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn Methods\|Distance spawn", meta = (AllowPrivateAccess = "true"))
		bool DistanceSpawnActivation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn Methods\|Distance spawn", meta = (AllowPrivateAccess = "true"))
		bool bDrawDistanceSpawn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn Methods\|Distance spawn", meta = (AllowPrivateAccess = "true"))
		int32 ObjectNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn Methods\|Distance spawn", meta = (AllowPrivateAccess = "true"))
		float DistBetwenItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn Methods\|Distance spawn\|SpawnArrange", meta = (AllowPrivateAccess = "true"))
		bool SpawnArrangeActivation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn Methods\|Distance spawn\|SpawnArrange", meta = (AllowPrivateAccess = "true"))
		float SpawnArrange;

	// Grid spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn Methods\|Grid spawn", meta = (AllowPrivateAccess = "true"))
		bool GridSpawnActivation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn Methods\|Grid spawn", meta = (AllowPrivateAccess = "true"))
		bool bDrawGridSpawn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn Methods\|Grid spawn", meta = (AllowPrivateAccess = "true"))
		int32 XGridSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn Methods\|Grid spawn", meta = (AllowPrivateAccess = "true"))
		int32 YGridSize;

	// Room splitting spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn Methods\|Floor split spawn", meta = (AllowPrivateAccess = "true"))
		bool FloorSplitSpawnActivation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn Methods\|Floor split spawn", meta = (AllowPrivateAccess = "true"))
		bool bDrawSplitSpawn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn Methods\|Floor split spawn", meta = (AllowPrivateAccess = "true"))
		float SplitValidationMetres;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn Methods\|Floor split spawn", meta = (AllowPrivateAccess = "true"))
		float SnapFactorMetres;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn Methods\|Floor split spawn\|Randomize Splitting", meta = (AllowPrivateAccess = "true"))
		bool RandomizeSplitting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Precedural Generation\|Spawn Methods\|Floor split spawn\|Randomize Splitting", meta = (AllowPrivateAccess = "true"))
		float SplitChance;

private:

	// Basic constructors

	void ItemArrayValidation();

	void SetProceduralVolumeScale();

	void GetInsetPoints();

	// Draw representation 

	void DrawDebugSquare(const FVector2D& TopLeft, const FVector2D& BottomRight, const float& Height, FColor Colore);

	// Spawn functions

	AActor* SpawnItemFromArr(TArray<TSubclassOf<AActor>> ItemArr, const FVector& Location, const FRotator& Rotation);

	void RandPositionSpawnArr(UClass* Item, int32 quantity, FVector ItemStartLocation, bool RandRotation = false);

	AActor* SpawnRandomItemInSquare(TArray<TSubclassOf<AActor>> ItemArr, const FVector2D& TopLeft, const FVector2D& BottomRight);

	AActor* SpawnSwitchMethod(TArray<TSubclassOf<AActor>> ItemArr, int32 ItemPosition, const FVector& Location, const FRotator& Rotation);

	// Item generators

	void GenerateFloor();

	void GenerateWalls();

	// Distance spawn

	void DrawDistanceSpawn(TArray<FVector>& SpawnedItems);

	void DistSpawn();

	// Grid spawn

	void DrawGridSpawn(TArray<FVector>& SpawnedItems, TArray<float>& Radius);

	void CreateLineGrid();

	void GridSpawn();

	// Room Separation spawn

	void FloorSplitSpawn();

};

