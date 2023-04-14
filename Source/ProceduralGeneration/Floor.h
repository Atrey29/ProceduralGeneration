// Andrii Kmet 

#pragma once

class FloorNode;

enum class ESplitOrientation
{
	ESO_Vertical,
	ESO_Horizontal,
	ESO_None
};

class  Floor
{
public:
	Floor(FVector2D UpperLeft, FVector2D BottomRight, float SplitValidation, float SnapFactor, bool RandSplit, float SplitChance);
	~Floor();

public:
	void Partition();

	FORCEINLINE TArray<TSharedPtr<FloorNode>> GetPartitionFloor() const { return PartitionedFloor; }

private:

	int32 SnapToGrid(float Point, float GridDivision);

	bool SizeValidation(TSharedPtr<FloorNode> Node);

	TPair<bool, ESplitOrientation> ShouldSplitNode(TSharedPtr<FloorNode> InNode);

	void SplitNode(TSharedPtr<FloorNode> InA, TSharedPtr<FloorNode> InB, TSharedPtr<FloorNode> InC, ESplitOrientation Orientation);

	void SplitHorizontal(TSharedPtr<FloorNode> InA, TSharedPtr<FloorNode> InB, TSharedPtr<FloorNode> InC);

	void SplitVertical(TSharedPtr<FloorNode> InA, TSharedPtr<FloorNode> InB, TSharedPtr<FloorNode> InC);

public:
	TArray<TSharedPtr<FloorNode>> PartitionedFloor;

private:
	TArray<TSharedPtr<FloorNode>> FloorNodeStack;

	FVector2D StartUpperLeft;
	FVector2D StartBottomRight;

	float SplitValidation;
	float SnapFactor;

	bool RandomSplit;
	float SplitChance;
};

