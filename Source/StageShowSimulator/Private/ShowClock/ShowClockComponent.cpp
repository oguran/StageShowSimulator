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
  AdvanceTime(DeltaTime);
}


void UShowClockComponent::AdvanceTime(float DeltaTimeSeconds)
{
  if (!bPlaying)
  {
    return;
  }

  CurrentTimeSeconds = ClampTime(CurrentTimeSeconds + (DeltaTimeSeconds * PlaybackRate));

  if (PlaybackLengthSeconds > 0.0f && CurrentTimeSeconds >= PlaybackLengthSeconds)
  {
    bPlaying = false;
  }
}

void UShowClockComponent::Play()
{
  if (PlaybackLengthSeconds > 0.0f && CurrentTimeSeconds >= PlaybackLengthSeconds)
  {
    CurrentTimeSeconds = 0.0f;
  }

  bPlaying = true;
}

void UShowClockComponent::Pause()
{
  bPlaying = false;
}

void UShowClockComponent::Stop()
{
  bPlaying = false;
  CurrentTimeSeconds = 0.0f;
}

void UShowClockComponent::Seek(float InTimeSeconds)
{
  CurrentTimeSeconds = ClampTime(InTimeSeconds);
}

float UShowClockComponent::GetCurrentTimeSeconds() const
{
  return CurrentTimeSeconds;
}

bool UShowClockComponent::IsPlaying() const
{
  return bPlaying;
}

float UShowClockComponent::ClampTime(float InTimeSeconds) const
{
  if (PlaybackLengthSeconds <= 0.0f)
  {
    return FMath::Max(0.0f, InTimeSeconds);
  }

  return FMath::Clamp(InTimeSeconds, 0.0f, PlaybackLengthSeconds);
}
