// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ArtNet/ArtNetTypes.h"

struct STAGESHOWSIMULATOR_API FArtNetPacketParser
{
  static bool TryParseDmx(const TArray<uint8>& PacketBytes, FArtNetDmxFrame& OutFrame, FString& OutError);
};
