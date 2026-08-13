// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace StageShowSimulatorMediaPlayback
{
  inline double ClampSeekTimeSeconds(
    double InTimeSeconds,
    double DurationSeconds)
  {
    constexpr double EndSafetyMarginSeconds = 0.1;

    // NaNやInfinityが入力された場合は、安全に0秒へ戻す
    if (!FMath::IsFinite(InTimeSeconds))
    {
      return 0.0;
    }

    // 負の時刻は0秒へクランプする
    const double NonNegativeTime =
      FMath::Max(0.0, InTimeSeconds);

    // Durationが有効な場合のみ上限クランプを行う
    if (FMath::IsFinite(DurationSeconds) &&
      DurationSeconds > 0.0)
    {
      // Duration以上へのSeekは、終端境界を避けて
      // Durationの0.1秒前へクランプする
      if (NonNegativeTime >= DurationSeconds)
      {
        return FMath::Max(
          0.0,
          DurationSeconds - EndSafetyMarginSeconds);
      }

      return NonNegativeTime;
    }

    // Durationを取得できない場合は上限クランプしない
    return NonNegativeTime;
  }
}
