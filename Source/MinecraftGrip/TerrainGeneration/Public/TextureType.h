#pragma once

#include "UObject/ObjectMacros.h"

UENUM()
enum class ETextureType : uint8
{
	StoneTexture UMETA(DisplayName = "StoneTexture"),
	DirtTexture UMETA(DisplayName = "DirtTexture"),
	GrassDirtTexture UMETA(DisplayName = "GrassDirtTexture"),
	SnowDirtTexture UMETA(DisplayName = "SnowDirtTexture"),
	SnowTexture UMETA(DisplayName = "SnowTexture"),
	WoodSideTexture UMETA(DisplayName = "WoodSideTexture"),
	WoodTopBottomTexture UMETA(DisplayName = "WoodTopBottomTexture"),
	GrassTopTexture UMETA(DisplayName = "GrassTopTexture"),
	BedrockTexture UMETA(DisplayName = "BedrockTexture"),
	SandTexture UMETA(DisplayName = "SandTexture"),
	GreenLeafTexture UMETA(DisplayName = "GreenLeafTexture"),
	BirchTopBottomTexture UMETA(DisplayName = "BirchTopBottomTexture"),
	BirchSideTexture UMETA(DisplayName = "BirchSideTexture"),
	BrownLeafTexture UMETA(DisplayName = "BirchLeafTexture")
};