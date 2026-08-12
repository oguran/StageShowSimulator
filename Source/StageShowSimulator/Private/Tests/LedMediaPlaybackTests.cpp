// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Media/LedMediaPlaybackComponent.h"
#include "Media/LedMediaPlaybackUtilities.h"
#include "MediaPlayer.h"
#include "Misc/AutomationTest.h"

#include <limits>

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLedMediaInitialStateTest, "StageShowSimulator.Media.InitialState", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLedMediaOpenWithoutPlayerTest, "StageShowSimulator.Media.OpenWithoutMediaPlayer", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLedMediaPlayWithoutPlayerTest, "StageShowSimulator.Media.PlayWithoutMediaPlayer", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLedMediaPauseWithoutPlayerTest, "StageShowSimulator.Media.PauseWithoutMediaPlayer", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLedMediaStopWithoutPlayerTest, "StageShowSimulator.Media.StopWithoutMediaPlayer", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLedMediaSeekWithoutPlayerTest, "StageShowSimulator.Media.SeekWithoutMediaPlayer", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLedMediaTimeAndDurationWithoutPlayerTest, "StageShowSimulator.Media.TimeAndDurationWithoutMediaPlayer", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLedMediaOpenWithoutSourceOrPathTest, "StageShowSimulator.Media.OpenWithoutMediaSourceOrPath", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLedMediaClampNegativeSeekTest, "StageShowSimulator.Media.ClampNegativeSeek", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLedMediaClampSeekToDurationTest, "StageShowSimulator.Media.ClampSeekToDuration", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLedMediaNoUpperClampWhenDurationIsZeroTest, "StageShowSimulator.Media.NoUpperClampWhenDurationIsZero", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLedMediaClampSeekInfinityInputTest, "StageShowSimulator.Media.ClampSeekInfinityInput", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLedMediaClampSeekNaNInputTest, "StageShowSimulator.Media.ClampSeekNaNInput", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FLedMediaInitialStateTest::RunTest(const FString& Parameters)
{
  ULedMediaPlaybackComponent* Component = NewObject<ULedMediaPlaybackComponent>();
  TestNotNull(TEXT("Component should be created"), Component);
  if (Component == nullptr)
  {
    return false;
  }

  TestFalse(TEXT("Initial bIsOpenPending should be false"), Component->bIsOpenPending);
  TestFalse(TEXT("Initial bIsOpened should be false"), Component->bIsOpened);
  TestFalse(TEXT("Initial MediaPlayer validity should be false"), Component->IsMediaPlayerValid());
  TestFalse(TEXT("Initial media readiness should be false"), Component->IsMediaReady());

  return true;
}

bool FLedMediaOpenWithoutPlayerTest::RunTest(const FString& Parameters)
{
  ULedMediaPlaybackComponent* Component = NewObject<ULedMediaPlaybackComponent>();
  TestFalse(TEXT("OpenMedia should fail when MediaPlayer is unset"), Component->OpenMedia());
  return true;
}

bool FLedMediaPlayWithoutPlayerTest::RunTest(const FString& Parameters)
{
  ULedMediaPlaybackComponent* Component = NewObject<ULedMediaPlaybackComponent>();
  TestFalse(TEXT("PlayMedia should fail when MediaPlayer is unset"), Component->PlayMedia());
  return true;
}

bool FLedMediaPauseWithoutPlayerTest::RunTest(const FString& Parameters)
{
  ULedMediaPlaybackComponent* Component = NewObject<ULedMediaPlaybackComponent>();
  TestFalse(TEXT("PauseMedia should fail when MediaPlayer is unset"), Component->PauseMedia());
  return true;
}

bool FLedMediaStopWithoutPlayerTest::RunTest(const FString& Parameters)
{
  ULedMediaPlaybackComponent* Component = NewObject<ULedMediaPlaybackComponent>();
  TestFalse(TEXT("StopMedia should fail when MediaPlayer is unset"), Component->StopMedia());
  return true;
}

bool FLedMediaSeekWithoutPlayerTest::RunTest(const FString& Parameters)
{
  ULedMediaPlaybackComponent* Component = NewObject<ULedMediaPlaybackComponent>();
  TestFalse(TEXT("SeekMedia should fail when MediaPlayer is unset"), Component->SeekMedia(1.0));
  return true;
}

bool FLedMediaTimeAndDurationWithoutPlayerTest::RunTest(const FString& Parameters)
{
  ULedMediaPlaybackComponent* Component = NewObject<ULedMediaPlaybackComponent>();
  TestEqual(TEXT("Current time should be zero when MediaPlayer is unset"), Component->GetCurrentMediaTimeSeconds(), 0.0);
  TestEqual(TEXT("Duration should be zero when MediaPlayer is unset"), Component->GetDurationSeconds(), 0.0);
  TestFalse(TEXT("MediaPlayer should be invalid when unset"), Component->IsMediaPlayerValid());
  TestFalse(TEXT("Media should not be ready when MediaPlayer is unset"), Component->IsMediaReady());
  return true;
}

bool FLedMediaOpenWithoutSourceOrPathTest::RunTest(const FString& Parameters)
{
  ULedMediaPlaybackComponent* Component = NewObject<ULedMediaPlaybackComponent>();
  TestNotNull(TEXT("Component should be created"), Component);
  if (Component == nullptr)
  {
    return false;
  }

  UMediaPlayer* Player = NewObject<UMediaPlayer>();
  TestNotNull(TEXT("MediaPlayer should be created"), Player);
  if (Player == nullptr)
  {
    return false;
  }

  Component->SetMediaPlayer(Player);
  Component->MediaSource = nullptr;
  Component->MediaFilePath.Empty();

  TestFalse(TEXT("OpenMedia should fail when MediaSource and MediaFilePath are both unset"), Component->OpenMedia());
  return true;
}

bool FLedMediaClampNegativeSeekTest::RunTest(const FString& Parameters)
{
  const double Clamped = StageShowSimulatorMediaPlayback::ClampSeekTimeSeconds(-5.0, 10.0);
  TestEqual(TEXT("Negative seek time should clamp to zero"), Clamped, 0.0);
  return true;
}

bool FLedMediaClampSeekToDurationTest::RunTest(const FString& Parameters)
{
  const double Clamped = StageShowSimulatorMediaPlayback::ClampSeekTimeSeconds(15.0, 10.0);
  TestEqual(TEXT("Seek time should clamp to duration when duration is positive"), Clamped, 10.0);
  return true;
}

bool FLedMediaNoUpperClampWhenDurationIsZeroTest::RunTest(const FString& Parameters)
{
  const double Clamped = StageShowSimulatorMediaPlayback::ClampSeekTimeSeconds(15.0, 0.0);
  TestEqual(TEXT("Seek time should not upper clamp when duration is zero"), Clamped, 15.0);
  return true;
}

bool FLedMediaClampSeekInfinityInputTest::RunTest(const FString& Parameters)
{
  const double Clamped = StageShowSimulatorMediaPlayback::ClampSeekTimeSeconds(std::numeric_limits<double>::infinity(), 10.0);
  TestEqual(TEXT("Infinity input should safely clamp to zero"), Clamped, 0.0);
  return true;
}

bool FLedMediaClampSeekNaNInputTest::RunTest(const FString& Parameters)
{
  const double Clamped = StageShowSimulatorMediaPlayback::ClampSeekTimeSeconds(std::numeric_limits<double>::quiet_NaN(), 10.0);
  TestEqual(TEXT("NaN input should safely clamp to zero"), Clamped, 0.0);
  return true;
}

#endif
