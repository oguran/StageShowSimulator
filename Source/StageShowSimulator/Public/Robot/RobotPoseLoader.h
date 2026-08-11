// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Robot/RobotPoseTypes.h"
#include "RobotPoseLoader.generated.h"

UCLASS()
class STAGESHOWSIMULATOR_API URobotPoseLoader : public UObject
{
  GENERATED_BODY()

public:
  UFUNCTION(BlueprintCallable, Category="RobotPose")
  static bool LoadPoseTrackFromJsonFile(const FString& JsonFilePath, FRobotPoseTrack& OutTrack, FString& OutError);
};
