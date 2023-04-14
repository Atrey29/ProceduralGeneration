#pragma once


struct FCornerCoordinates
{
	int32 UpperLeftX; 
	int32 UpperLeftY;

	int32 LoverRightX;
	int32 LoverRightY;
};

class FloorNode
{
public:
	FloorNode();
	FloorNode(const FCornerCoordinates& CornerCoordinates);

	~FloorNode();

	FORCEINLINE FCornerCoordinates GetCornerCordinates() const { return CornerCoordinates; }
	FORCEINLINE void SetCornerCordinates(FCornerCoordinates Coordinates) { this->CornerCoordinates = Coordinates; }

	FORCEINLINE static int32 GetFloorNodeCount() { return FloorNodeCount; }

private:
	FCornerCoordinates CornerCoordinates;

	inline static int32 FloorNodeCount = 0;
};



