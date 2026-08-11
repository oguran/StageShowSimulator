// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShowClock/ShowClockComponent.h"

UShowClockComponent::UShowClockComponent()
{
  PrimaryComponentTick.bCanEverTick = true;
}

void UShowClockComponent::BeginPlay()
{
  Super::BeginPlay();

  if (bAutoPlay)
  {
    Play();
  }
}

void UShowClockComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
  AdvanceTime(static_cast<double>(DeltaTime));
}


void UShowClockComponent::AdvanceTime(double DeltaTimeSeconds)
{
  if (State != EShowClockState::Playing)
  {
    return;
  }

  CurrentTimeSeconds = ClampTime(CurrentTimeSeconds + (DeltaTimeSeconds * PlaybackRate));

  if (PlaybackLengthSeconds > 0.0 && CurrentTimeSeconds >= PlaybackLengthSeconds)
  {
    State = EShowClockState::Paused;
  }
}

void UShowClockComponent::Play()
{
  if (PlaybackLengthSeconds > 0.0 && CurrentTimeSeconds >= PlaybackLengthSeconds)
  {
    CurrentTimeSeconds = 0.0;
  }

  State = EShowClockState::Playing;
}

void UShowClockComponent::Pause()
{
  if (State == EShowClockState::Playing || State == EShowClockState::Seeking)
  {
    State = EShowClockState::Paused;
  }
}

void UShowClockComponent::Stop()
{
  CurrentTimeSeconds = 0.0;
  State = EShowClockState::Stopped;
}

void UShowClockComponent::Seek(double InTimeSeconds)
{
  const EShowClockState PreviousState = State;
  State = EShowClockState::Seeking;
  CurrentTimeSeconds = ClampTime(InTimeSeconds);

  if (PreviousState == EShowClockState::Playing)
  {
    State = EShowClockState::Playing;
  }
  else if (CurrentTimeSeconds <= 0.0)
  {
    State = EShowClockState::Stopped;
  }
  else
  {
    State = EShowClockState::Paused;
  }
}

double UShowClockComponent::GetCurrentTimeSeconds() const
{
  return CurrentTimeSeconds;
}

bool UShowClockComponent::IsPlaying() const
{
  return State == EShowClockState::Playing;
}

EShowClockState UShowClockComponent::GetState() const
{
  return State;
}

double UShowClockComponent::ClampTime(double InTimeSeconds) const
{
  if (PlaybackLengthSeconds <= 0.0)
  {
    return FMath::Max(0.0, InTimeSeconds);
  }

  return FMath::Clamp(InTimeSeconds, 0.0, PlaybackLengthSeconds);
}
