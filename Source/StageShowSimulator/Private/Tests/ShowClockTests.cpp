// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "ShowClock/ShowClockComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FShowClockSeekBoundaryTest, "StageShowSimulator.ShowClock.SeekBoundary", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FShowClockSeekBoundaryTest::RunTest(const FString& Parameters)
{
  UShowClockComponent* Clock = NewObject<UShowClockComponent>();
  TestNotNull(TEXT("Clock should be created"), Clock);
  if (Clock == nullptr)
  {
    return false;
  }

  Clock->PlaybackLengthSeconds = 10.0f;
  TestTrue(TEXT("Initial state should be Stopped"), Clock->GetState() == EShowClockState::Stopped);

  Clock->Seek(-2.0f);
  TestTrue(TEXT("Seek should clamp to zero"), Clock->GetCurrentTimeSeconds() == 0.0);
  TestTrue(TEXT("Seek to zero should resolve to Stopped"), Clock->GetState() == EShowClockState::Stopped);

  Clock->Play();
  TestTrue(TEXT("Play should set Playing state"), Clock->GetState() == EShowClockState::Playing);
 Clock->AdvanceTime(1.5f);
  TestTrue(TEXT("Clock should advance while playing"), Clock->GetCurrentTimeSeconds() == 1.5);

  Clock->Seek(999.0f);
  TestTrue(TEXT("Seek should clamp to end"), Clock->GetCurrentTimeSeconds() == 10.0);
  TestTrue(TEXT("Seek while playing should keep Playing state"), Clock->GetState() == EShowClockState::Playing);

 Clock->AdvanceTime(1.0f);
  TestFalse(TEXT("Clock should stop at end"), Clock->IsPlaying());
  TestTrue(TEXT("Clock should enter Paused state at end"), Clock->GetState() == EShowClockState::Paused);

  Clock->Pause();
  TestTrue(TEXT("Pause should keep Paused state"), Clock->GetState() == EShowClockState::Paused);

  Clock->Stop();
  TestTrue(TEXT("Stop should reset time"), Clock->GetCurrentTimeSeconds() == 0.0);
  TestTrue(TEXT("Stop should set Stopped state"), Clock->GetState() == EShowClockState::Stopped);

  return true;
}

#endif
