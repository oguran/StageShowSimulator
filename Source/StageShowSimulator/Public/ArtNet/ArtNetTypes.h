// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ArtNetTypes.generated.h"

USTRUCT(BlueprintType)
struct STAGESHOWSIMULATOR_API FArtNetDmxFrame
{
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ArtNet")
  int32 Universe = 0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ArtNet")
  int32 Sequence = 0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ArtNet")
  TArray<uint8> Channels;
};
