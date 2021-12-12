#pragma once

UENUM()
enum class EBlockType : uint8
{
	AirBlock UMETA(DisplayName = "AirBlock"),
	BedrockBlock UMETA(DisplayName = "BedrockBlock"),
	StoneBlock UMETA(DisplayName = "StoneBlock"),
	GrassBlock UMETA(DisplayName = "GrassBlock"),
	DirtBlock UMETA(DisplayName = "DirtBlock"),
	SandBlock UMETA(DisplayName = "SandBlock"),
	SnowDirtBlock UMETA(DisplayName = "SnowDirtBlock"),
	SnowBlock UMETA(DisplayName = "SnowBlock"),
	WoodBlock UMETA(DisplayName = "WoodBlock"),
	BirchWood UMETA(DisplayName = "BirchWood"),
	GreenLeaves UMETA(DisplayName = "GreenLeaves"),
	BrownLeaves UMETA(DisplayName = "BrownLeaves")
};