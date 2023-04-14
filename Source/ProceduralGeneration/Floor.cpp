// Andrii Kmet 

#include "Floor.h"
#include "FloorNode.h"

Floor::Floor(FVector2D UpperLeft, FVector2D BottomRight, float SplitValidation, float SnapFactor, bool RandSplit, float SplitChance)
{
	this->StartUpperLeft = UpperLeft;
	this->StartBottomRight = BottomRight;
	this->SplitValidation = SplitValidation;
	this->SnapFactor = SnapFactor;
	this->RandomSplit = RandSplit;
	this->SplitChance = SplitChance;
}

Floor::~Floor()
{
}

int32 Floor::SnapToGrid(float Point, float GridDivision)
{
	int32 RoundedValue = FMath::RoundToInt(Point);
	int32 SnapValue = FMath::RoundToInt(RoundedValue / GridDivision) * GridDivision;
	if (FMath::Abs(SnapValue - RoundedValue) > FMath::Abs(GridDivision / 2)) {
		SnapValue += (GridDivision > 0) ? GridDivision : -GridDivision;
	}
	return SnapValue;
}

bool Floor::SizeValidation(TSharedPtr<FloorNode> Node)
{
	if(Node->GetCornerCordinates().LoverRightX == Node->GetCornerCordinates().UpperLeftX ||
	   Node->GetCornerCordinates().LoverRightY == Node->GetCornerCordinates().UpperLeftY) {
		return false;
	}
	return true;
}

void Floor::Partition()
{
	FCornerCoordinates CornerCodinates = {static_cast<int32>(StartUpperLeft.X),
										  static_cast<int32>(StartUpperLeft.Y),
										  static_cast<int32>(StartBottomRight.X),
										  static_cast<int32>(StartBottomRight.Y) };

	FloorNodeStack.Push(MakeShared<FloorNode>(CornerCodinates));

	while (!FloorNodeStack.IsEmpty()) { 
		const TSharedPtr<FloorNode> AFloorNodePtr = FloorNodeStack.Pop();
		const TPair<bool, ESplitOrientation> SplitVerification = ShouldSplitNode(AFloorNodePtr);

		if (SplitVerification.Key)
		{
			TSharedPtr<FloorNode> BFloorNodePtr = MakeShared<FloorNode>();
			TSharedPtr<FloorNode> CFloorNodePtr = MakeShared<FloorNode>();

			SplitNode(AFloorNodePtr, BFloorNodePtr, CFloorNodePtr, SplitVerification.Value);

			FloorNodeStack.Push(BFloorNodePtr);
			FloorNodeStack.Push(CFloorNodePtr);
		}
		else
		{
			if(SizeValidation(AFloorNodePtr)) {
				PartitionedFloor.Push(AFloorNodePtr);
			}
		}
	}
}

TPair<bool, ESplitOrientation> Floor::ShouldSplitNode(TSharedPtr<FloorNode> InNode)
{
	FCornerCoordinates CornerCoordinates = InNode->GetCornerCordinates();

	const float FloorSideX = FMath::Abs(StartUpperLeft.X - StartBottomRight.X);
	const float FloorSideY = FMath::Abs(StartUpperLeft.Y - StartBottomRight.Y);

	const float SideX = FMath::Abs(CornerCoordinates.UpperLeftX - CornerCoordinates.LoverRightX);
	const float SideY = FMath::Abs(CornerCoordinates.UpperLeftY - CornerCoordinates.LoverRightY);

	const bool CheckXSize = SideX > SplitValidation;
	const bool CheckYSize = SideY > SplitValidation;

	const float DiceRoll = FMath::FRandRange(0.f, 1.f);

	if(CheckXSize && !CheckYSize) {
		if(RandomSplit) {
			float PercentChanceOfSplit = (SideX / FloorSideX) * SplitChance;
			if(DiceRoll > PercentChanceOfSplit) {
				return TPair<bool, ESplitOrientation>(false, ESplitOrientation::ESO_None);
			}
			else {
				return TPair<bool, ESplitOrientation>(true, ESplitOrientation::ESO_Horizontal);
			}
		}
		else {
			return TPair<bool, ESplitOrientation>(true, ESplitOrientation::ESO_Horizontal);
		}
	}
	else if(!CheckXSize && CheckYSize) {
		if(RandomSplit) {
			float PercentChanceOfSplit = (SideY / FloorSideY) * SplitChance;
			if(DiceRoll > PercentChanceOfSplit) {
				return TPair<bool, ESplitOrientation>(false, ESplitOrientation::ESO_None);
			}
			else {
				return TPair<bool, ESplitOrientation>(true, ESplitOrientation::ESO_Vertical);
			}
		}
		else {
			return TPair<bool, ESplitOrientation>(true, ESplitOrientation::ESO_Vertical);
		}
	}
	else if(CheckXSize && CheckYSize) {
		if(RandomSplit) {
			TPair<ESplitOrientation, float> PercentChanceOfSplit = ((SideX / FloorSideX) > (SideY / FloorSideY)) ? TPair<ESplitOrientation, float>{ESplitOrientation::ESO_Horizontal,(SideX / FloorSideX)} : TPair<ESplitOrientation, float>{ ESplitOrientation::ESO_Vertical,(SideY / FloorSideY) };
			PercentChanceOfSplit.Value *= SplitChance;

			if(PercentChanceOfSplit.Key == ESplitOrientation::ESO_Horizontal) {
				if (DiceRoll > PercentChanceOfSplit.Value) {
					return TPair<bool, ESplitOrientation>(false, ESplitOrientation::ESO_None);
				}
				else {
					return TPair<bool, ESplitOrientation>(true, ESplitOrientation::ESO_Horizontal);
				}
			}
			else {
				if (DiceRoll > PercentChanceOfSplit.Value) {
					return TPair<bool, ESplitOrientation>(false, ESplitOrientation::ESO_None);
				}
				else {
					return TPair<bool, ESplitOrientation>(true, ESplitOrientation::ESO_Vertical);
				}
			}
		}
		else {
			return TPair<bool, ESplitOrientation>(true, static_cast<ESplitOrientation>(FMath::RandBool()));
		}
	}
	else {
		return TPair<bool, ESplitOrientation>(false, ESplitOrientation::ESO_None);
	}
}

void Floor::SplitNode(TSharedPtr<FloorNode> InA, TSharedPtr<FloorNode> InB, TSharedPtr<FloorNode> InC, ESplitOrientation Orientation)
{
	if(Orientation == ESplitOrientation::ESO_Vertical) {
		SplitVertical(InA, InB, InC);
	}
	else {
		SplitHorizontal(InA, InB, InC);
	}
}

void Floor::SplitVertical(TSharedPtr<FloorNode> InA, TSharedPtr<FloorNode> InB, TSharedPtr<FloorNode> InC)
{
	FCornerCoordinates CurrentNode = InA->GetCornerCordinates();
	float MinValue = static_cast<float>(CurrentNode.UpperLeftY);
	float MaxValue = static_cast<float>(CurrentNode.LoverRightY);

	float Separation = FMath::RandRange(MinValue, MaxValue);
	int32 SplitPointY = SnapToGrid(Separation, SnapFactor);

	InB->SetCornerCordinates(FCornerCoordinates{ CurrentNode.UpperLeftX, CurrentNode.UpperLeftY, CurrentNode.LoverRightX, SplitPointY });
	InC->SetCornerCordinates(FCornerCoordinates{ CurrentNode.UpperLeftX, SplitPointY, CurrentNode.LoverRightX,CurrentNode.LoverRightY });
}

void Floor::SplitHorizontal(TSharedPtr<FloorNode> InA, TSharedPtr<FloorNode> InB, TSharedPtr<FloorNode> InC)
{
	FCornerCoordinates CurrentNode = InA->GetCornerCordinates();
	float MinValue = static_cast<float>(CurrentNode.UpperLeftX);
	float MaxValue = static_cast<float>(CurrentNode.LoverRightX);

	float Separation = FMath::RandRange(MinValue,MaxValue);
	int32 SplitPointX = SnapToGrid(Separation, SnapFactor);

	InB->SetCornerCordinates(FCornerCoordinates{ CurrentNode.UpperLeftX, CurrentNode.UpperLeftY, SplitPointX, CurrentNode.LoverRightY });
	InC->SetCornerCordinates(FCornerCoordinates{ SplitPointX, CurrentNode.UpperLeftY, CurrentNode.LoverRightX,CurrentNode.LoverRightY });
}


