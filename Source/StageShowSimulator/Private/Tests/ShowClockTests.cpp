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
  Clock->Seek(-2.0f);
  TestEqual(TEXT("Seek should clamp to zero"), Clock->GetCurrentTimeSeconds(), 0.0f);

  Clock->Play();
 Clock->AdvanceTime(1.5f);
  TestEqual(TEXT("Clock should advance while playing"), Clock->GetCurrentTimeSeconds(), 1.5f);

  Clock->Seek(999.0f);
  TestEqual(TEXT("Seek should clamp to end"), Clock->GetCurrentTimeSeconds(), 10.0f);

 Clock->AdvanceTime(1.0f);
  TestFalse(TEXT("Clock should stop at end"), Clock->IsPlaying());

  Clock->Stop();
  TestEqual(TEXT("Stop should reset time"), Clock->GetCurrentTimeSeconds(), 0.0f);

  return true;
}

#endif
