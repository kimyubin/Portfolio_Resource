#pragma once

UENUM(BlueprintType)
enum class EVXDirection : uint8
{
	Forward, Right, Back, Left, Up, Down
};

UENUM(BlueprintType)
enum class EBlockType : uint8
{
	Null  = 0
  , Air   = 1
  , Stone = 2
  , Dirt  = 3
  , Grass = 4
};

struct FBlockInfo
{
	float Weight;
	EBlockType BlockType;
};
