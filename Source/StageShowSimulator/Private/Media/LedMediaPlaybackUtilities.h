// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace StageShowSimulatorMediaPlayback
{
  inline double ClampSeekTimeSeconds(double InTimeSeconds, double DurationSeconds)
  {
    if (!FMath::IsFinite(InTimeSeconds))
    {
      return 0.0;
    }

    const double NonNegativeTime = FMath::Max(0.0, InTimeSeconds);
    if (FMath::IsFinite(DurationSeconds) && DurationSeconds > 0.0)
    {
      return FMath::Min(NonNegativeTime, DurationSeconds);
    }

    return NonNegativeTime;
  }
}
