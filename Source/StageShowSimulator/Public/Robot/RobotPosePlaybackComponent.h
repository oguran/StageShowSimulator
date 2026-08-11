// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Robot/RobotPoseTypes.h"
#include "RobotPosePlaybackComponent.generated.h"

class UShowClockComponent;

UCLASS(ClassGroup=(StageShow), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class STAGESHOWSIMULATOR_API URobotPosePlaybackComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  URobotPosePlaybackComponent();

  virtual void BeginPlay() override;
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

  UFUNCTION(BlueprintCallable, Category="RobotPose")
  bool LoadTrack();

  UFUNCTION(BlueprintCallable, Category="RobotPose")
  bool EvaluateAtTime(double TimeSeconds, FRobotPoseSample& OutPose) const;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RobotPose")
  FString JsonFilePath;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RobotPose")
  bool bLoadOnBeginPlay = true;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RobotPose")
  bool bApplyToOwnerActor = true;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RobotPose")
  TObjectPtr<UShowClockComponent> ShowClock = nullptr;

  UPROPERTY(BlueprintReadOnly, Category="RobotPose")
  FRobotPoseSample CurrentPose;

private:
  FRobotPoseTrack Track;
  bool bTrackLoaded = false;
};
