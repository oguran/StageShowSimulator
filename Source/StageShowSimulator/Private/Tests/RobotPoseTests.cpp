// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "HAL/PlatformFileManager.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Robot/RobotPoseLoader.h"
#include "Robot/RobotPoseTypes.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRobotPoseJsonValidationTest, "StageShowSimulator.RobotPose.JsonValidation", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRobotPoseInterpolationTest, "StageShowSimulator.RobotPose.Interpolation", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRobotPoseBoundaryTest, "StageShowSimulator.RobotPose.Boundary", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRobotPoseShortestYawInterpolationTest, "StageShowSimulator.RobotPose.ShortestYawInterpolation", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRobotPoseJsonPathValidationTest, "StageShowSimulator.RobotPose.JsonPathValidation", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRobotPoseUnitConversionTest, "StageShowSimulator.RobotPose.UnitConversion", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRobotPoseJsonValidationTest::RunTest(const FString& Parameters)
{
  FRobotPoseTrack Track;
  FRobotPoseSample A;
  A.TimeSeconds = 1.0;
  FRobotPoseSample B;
  B.TimeSeconds = 0.5;
  Track.Samples = { A, B };

  FString Error;
  TestFalse(TEXT("Track with non-increasing time should be invalid"), Track.Validate(Error));
  TestFalse(TEXT("Validation should return message"), Error.IsEmpty());
  return true;
}

bool FRobotPoseInterpolationTest::RunTest(const FString& Parameters)
{
  FRobotPoseTrack Track;

  FRobotPoseSample Start;
  Start.TimeSeconds = 0.0;
  Start.Position2D = FVector2D(0.0f, 0.0f);
  Start.YawDegrees = 0.0f;

  FRobotPoseSample End;
  End.TimeSeconds = 10.0;
  End.Position2D = FVector2D(10.0f, 20.0f);
  End.YawDegrees = 90.0f;

  Track.Samples = { Start, End };

  FRobotPoseSample Result;
  TestTrue(TEXT("Evaluate should succeed"), Track.EvaluatePose(5.0, Result));
 TestEqual(TEXT("Interpolated X"), Result.Position2D.X, 5.0);
  TestEqual(TEXT("Interpolated Y"), Result.Position2D.Y, 10.0);
  TestEqual(TEXT("Interpolated Yaw"), Result.YawDegrees, 45.0f);
  return true;
}

bool FRobotPoseBoundaryTest::RunTest(const FString& Parameters)
{
  FRobotPoseTrack Track;

  FRobotPoseSample First;
  First.TimeSeconds = 1.0;
 First.Position2D = FVector2D(1.0, 2.0);

  FRobotPoseSample Last;
  Last.TimeSeconds = 3.0;
  Last.Position2D = FVector2D(7.0, 8.0);

  Track.Samples = { First, Last };

  FRobotPoseSample Result;
  TestTrue(TEXT("Before range should evaluate"), Track.EvaluatePose(0.0, Result));
  TestEqual(TEXT("Before range uses first pose X"), Result.Position2D.X, First.Position2D.X);

  TestTrue(TEXT("After range should evaluate"), Track.EvaluatePose(9.0, Result));
  TestEqual(TEXT("After range uses last pose X"), Result.Position2D.X, Last.Position2D.X);

  return true;
}

bool FRobotPoseShortestYawInterpolationTest::RunTest(const FString& Parameters)
{
  FRobotPoseTrack Track;

  FRobotPoseSample Start;
  Start.TimeSeconds = 0.0;
  Start.YawDegrees = 350.0f;

  FRobotPoseSample End;
  End.TimeSeconds = 10.0;
  End.YawDegrees = 10.0f;

  Track.Samples = { Start, End };

  FRobotPoseSample Result;
  TestTrue(TEXT("Evaluate should succeed for shortest-yaw interpolation"), Track.EvaluatePose(5.0, Result));
  TestEqual(TEXT("Shortest yaw interpolation should cross zero"), Result.YawDegrees, 360.0f);

  return true;
}

bool FRobotPoseJsonPathValidationTest::RunTest(const FString& Parameters)
{
  FRobotPoseTrack Track;
  FString Error;

  AddExpectedError(TEXT("Invalid JSON file path"), EAutomationExpectedErrorFlags::Contains, 1);
  TestFalse(TEXT("Invalid path should fail validation"), URobotPoseLoader::LoadPoseTrackFromJsonFile(TEXT("?:/invalid<>path.json"), Track, Error));
  TestFalse(TEXT("Invalid path should produce error message"), Error.IsEmpty());

  const FString RelativePath = TEXT("Saved/Tests/RobotPoseRelativePathTest.json");
  const FString AbsolutePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir(), RelativePath);
  IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
  PlatformFile.CreateDirectoryTree(*FPaths::GetPath(AbsolutePath));

  const FString JsonText = TEXT("{\"samples\":[{\"time\":0.0,\"x\":1.0,\"y\":2.0,\"yaw\":45.0},{\"time\":1.0,\"x\":3.0,\"y\":4.0,\"yaw\":90.0}]}");
  TestTrue(TEXT("Test JSON file should be created"), FFileHelper::SaveStringToFile(JsonText, *AbsolutePath));

  Error.Empty();
  TestTrue(TEXT("Relative project path should load successfully"), URobotPoseLoader::LoadPoseTrackFromJsonFile(RelativePath, Track, Error));

  PlatformFile.DeleteFile(*AbsolutePath);
  return true;
}

bool FRobotPoseUnitConversionTest::RunTest(const FString& Parameters)
{
  const FString RelativePath = TEXT("Saved/Tests/RobotPoseUnitConversionTest.json");
  const FString AbsolutePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir(), RelativePath);
  IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
  PlatformFile.CreateDirectoryTree(*FPaths::GetPath(AbsolutePath));

  const FString JsonText = TEXT("{\"samples\":[{\"time\":0.0,\"x\":1.0,\"y\":2.0,\"yaw\":0.0}]}");
  TestTrue(TEXT("Unit conversion test JSON should be created"), FFileHelper::SaveStringToFile(JsonText, *AbsolutePath));

  FRobotPoseTrack Track;
  FString Error;
  const bool bLoaded = URobotPoseLoader::LoadPoseTrackFromJsonFile(RelativePath, Track, Error);
  TestTrue(TEXT("Track should load successfully"), bLoaded);
  if (bLoaded)
  {
    TestTrue(TEXT("X should convert meter to centimeter"), Track.Samples[0].Position2D.X == 100.0f);
    TestTrue(TEXT("Y should convert meter to centimeter"), Track.Samples[0].Position2D.Y == 200.0f);
  }

  PlatformFile.DeleteFile(*AbsolutePath);
  return true;
}

#endif
