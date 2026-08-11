// Copyright Epic Games, Inc. All Rights Reserved.

#include "Robot/RobotPosePlaybackComponent.h"

#include "GameFramework/Actor.h"
#include "Robot/RobotPoseLoader.h"
#include "ShowClock/ShowClockComponent.h"
#include "StageShowSimulatorLog.h"

URobotPosePlaybackComponent::URobotPosePlaybackComponent()
{
  PrimaryComponentTick.bCanEverTick = true;
}

void URobotPosePlaybackComponent::BeginPlay()
{
  Super::BeginPlay();

  if (bLoadOnBeginPlay)
  {
    LoadTrack();
  }
}

void URobotPosePlaybackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  if (!bTrackLoaded || ShowClock == nullptr)
  {
    return;
  }

  if (!Track.EvaluatePose(ShowClock->GetCurrentTimeSeconds(), CurrentPose))
  {
    return;
  }

  if (!bApplyToOwnerActor)
  {
    return;
  }

  AActor* Owner = GetOwner();
  if (Owner == nullptr)
  {
    return;
  }

  const FVector CurrentLocation = Owner->GetActorLocation();
  const FVector NewLocation(CurrentPose.Position2D.X, CurrentPose.Position2D.Y, CurrentLocation.Z);
  Owner->SetActorLocation(NewLocation);

  FRotator Rotation = Owner->GetActorRotation();
  Rotation.Yaw = CurrentPose.YawDegrees;
  Owner->SetActorRotation(Rotation);
}

bool URobotPosePlaybackComponent::LoadTrack()
{
  if (JsonFilePath.IsEmpty())
  {
    UE_LOG(LogStageShowSimulator, Warning, TEXT("RobotPosePlaybackComponent: JsonFilePath is empty."));
    bTrackLoaded = false;
    return false;
  }

  FString Error;
  bTrackLoaded = URobotPoseLoader::LoadPoseTrackFromJsonFile(JsonFilePath, Track, Error);
  if (!bTrackLoaded)
  {
    UE_LOG(LogStageShowSimulator, Error, TEXT("Robot pose track load failed: %s"), *Error);
  }

  return bTrackLoaded;
}

bool URobotPosePlaybackComponent::EvaluateAtTime(double TimeSeconds, FRobotPoseSample& OutPose) const
{
  if (!bTrackLoaded)
  {
    return false;
  }

  return Track.EvaluatePose(TimeSeconds, OutPose);
}
