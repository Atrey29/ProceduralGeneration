#include "FloorNode.h"


FloorNode::FloorNode()
{
	++FloorNodeCount;
}

FloorNode::FloorNode(const FCornerCoordinates& Coordinates)
{
	this->CornerCoordinates.UpperLeftX = Coordinates.UpperLeftX;
	this->CornerCoordinates.UpperLeftY = Coordinates.UpperLeftY;
	this->CornerCoordinates.LoverRightX = Coordinates.LoverRightX;
	this->CornerCoordinates.LoverRightY = Coordinates.LoverRightY;

	++FloorNodeCount;
}

FloorNode::~FloorNode()
{
	--FloorNodeCount;
}
