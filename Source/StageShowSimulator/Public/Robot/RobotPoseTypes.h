// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RobotPoseTypes.generated.h"

USTRUCT(BlueprintType)
struct STAGESHOWSIMULATOR_API FRobotPoseSample
{
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RobotPose")
  double TimeSeconds = 0.0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RobotPose")
  FVector2D Position2D = FVector2D::ZeroVector;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RobotPose")
  float YawDegrees = 0.0f;
};

USTRUCT(BlueprintType)
struct STAGESHOWSIMULATOR_API FRobotPoseTrack
{
 GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RobotPose")
  TArray<FRobotPoseSample> Samples;

  bool Validate(FString& OutError) const
  {
    if (Samples.IsEmpty())
    {
      OutError = TEXT("Samples is empty.");
      return false;
    }

    double PreviousTime = -1.0;
    for (int32 Index = 0; Index < Samples.Num(); ++Index)
    {
      const FRobotPoseSample& Sample = Samples[Index];
      if (!FMath::IsFinite(Sample.TimeSeconds))
      {
        OutError = FString::Printf(TEXT("Sample[%d] has non-finite TimeSeconds."), Index);
        return false;
      }

      if (Sample.TimeSeconds < 0.0)
      {
        OutError = FString::Printf(TEXT("Sample[%d] has negative TimeSeconds."), Index);
        return false;
      }

      if (Index > 0 && Sample.TimeSeconds <= PreviousTime)
      {
        OutError = FString::Printf(TEXT("Sample[%d] time is not strictly increasing."), Index);
        return false;
      }

      PreviousTime = Sample.TimeSeconds;
    }

    OutError.Empty();
    return true;
  }

  bool EvaluatePose(double TimeSeconds, FRobotPoseSample& OutPose) const
  {
    if (Samples.IsEmpty())
    {
      return false;
    }

    if (TimeSeconds <= Samples[0].TimeSeconds)
    {
      OutPose = Samples[0];
      OutPose.TimeSeconds = TimeSeconds;
      return true;
    }

    const FRobotPoseSample& Last = Samples.Last();
    if (TimeSeconds >= Last.TimeSeconds)
    {
      OutPose = Last;
      OutPose.TimeSeconds = TimeSeconds;
      return true;
    }

    int32 LowerIndex = 0;
    int32 UpperIndex = Samples.Num() - 1;
    while (LowerIndex + 1 < UpperIndex)
    {
      const int32 MidIndex = LowerIndex + ((UpperIndex - LowerIndex) / 2);
      if (TimeSeconds < Samples[MidIndex].TimeSeconds)
      {
        UpperIndex = MidIndex;
      }
      else
      {
        LowerIndex = MidIndex;
      }
    }

    for (int32 Index = LowerIndex; Index <= LowerIndex + 1 && Index < Samples.Num() - 1; ++Index)
    {
      const FRobotPoseSample& A = Samples[Index];
      const FRobotPoseSample& B = Samples[Index + 1];
      if (TimeSeconds >= A.TimeSeconds && TimeSeconds <= B.TimeSeconds)
      {
        const double Range = B.TimeSeconds - A.TimeSeconds;
        const double Alpha = Range > 0.0 ? (TimeSeconds - A.TimeSeconds) / Range : 0.0;
        const float DeltaYaw = FMath::FindDeltaAngleDegrees(A.YawDegrees, B.YawDegrees);

        OutPose.TimeSeconds = TimeSeconds;
        OutPose.Position2D = FMath::Lerp(A.Position2D, B.Position2D, Alpha);
        OutPose.YawDegrees = A.YawDegrees + (DeltaYaw * static_cast<float>(Alpha));
        return true;
      }
    }

    return false;
  }
};
