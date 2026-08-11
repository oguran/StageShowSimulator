// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright Epic Games, Inc. All Rights Reserved.

#include "Robot/RobotPoseLoader.h"

#include "Dom/JsonObject.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "StageShowSimulatorLog.h"

namespace
{
  constexpr float MetersToCentimeters = 100.0f;
}

bool URobotPoseLoader::LoadPoseTrackFromJsonFile(const FString& JsonFilePath, FRobotPoseTrack& OutTrack, FString& OutError)
{
  OutTrack.Samples.Reset();
  OutError.Empty();

  FString NormalizedPath = JsonFilePath;
  FPaths::NormalizeFilename(NormalizedPath);

  FText PathValidationError;
  if (!FPaths::ValidatePath(NormalizedPath, &PathValidationError))
  {
    OutError = FString::Printf(TEXT("Invalid JSON file path: %s (%s)"), *JsonFilePath, *PathValidationError.ToString());
    UE_LOG(LogStageShowSimulator, Error, TEXT("%s"), *OutError);
    return false;
  }

  if (FPaths::IsRelative(NormalizedPath))
  {
    NormalizedPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir(), NormalizedPath);
    FPaths::NormalizeFilename(NormalizedPath);
  }

  if (!FPaths::FileExists(NormalizedPath))
  {
    OutError = FString::Printf(TEXT("JSON file does not exist: %s"), *NormalizedPath);
    UE_LOG(LogStageShowSimulator, Error, TEXT("%s"), *OutError);
    return false;
  }

  FString JsonText;
  if (!FFileHelper::LoadFileToString(JsonText, *NormalizedPath))
  {
    OutError = FString::Printf(TEXT("Failed to read file: %s"), *NormalizedPath);
    UE_LOG(LogStageShowSimulator, Error, TEXT("%s"), *OutError);
    return false;
  }

  TSharedPtr<FJsonObject> RootObject;
  const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);
  if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
  {
    OutError = TEXT("Failed to parse JSON root object.");
    UE_LOG(LogStageShowSimulator, Error, TEXT("%s"), *OutError);
    return false;
  }

  const TArray<TSharedPtr<FJsonValue>>* SamplesArray = nullptr;
  if (!RootObject->TryGetArrayField(TEXT("samples"), SamplesArray) || SamplesArray == nullptr)
  {
    OutError = TEXT("Missing 'samples' array.");
    UE_LOG(LogStageShowSimulator, Error, TEXT("%s"), *OutError);
    return false;
  }

  OutTrack.Samples.Reserve(SamplesArray->Num());

 for (int32 Index = 0; Index < SamplesArray->Num(); ++Index)
  {
   const TSharedPtr<FJsonObject> SampleObject = (*SamplesArray)[Index].IsValid() ? (*SamplesArray)[Index]->AsObject() : nullptr;
    if (!SampleObject.IsValid())
    {
      OutError = FString::Printf(TEXT("Sample[%d] is not an object."), Index);
      UE_LOG(LogStageShowSimulator, Error, TEXT("%s"), *OutError);
      return false;
    }

    double TimeSeconds = 0.0;
    double X = 0.0;
    double Y = 0.0;
    double YawDegrees = 0.0;

    if (!SampleObject->TryGetNumberField(TEXT("time"), TimeSeconds)
      || !SampleObject->TryGetNumberField(TEXT("x"), X)
      || !SampleObject->TryGetNumberField(TEXT("y"), Y)
      || !SampleObject->TryGetNumberField(TEXT("yaw"), YawDegrees))
    {
      OutError = FString::Printf(TEXT("Sample[%d] is missing one of required fields: time, x, y, yaw."), Index);
      UE_LOG(LogStageShowSimulator, Error, TEXT("%s"), *OutError);
      return false;
    }

    FRobotPoseSample Sample;
    Sample.TimeSeconds = TimeSeconds;
    Sample.Position2D = FVector2D(static_cast<float>(X) * MetersToCentimeters, static_cast<float>(Y) * MetersToCentimeters);
    Sample.YawDegrees = static_cast<float>(YawDegrees);
    OutTrack.Samples.Add(Sample);
  }

  if (!OutTrack.Validate(OutError))
  {
    UE_LOG(LogStageShowSimulator, Error, TEXT("Robot pose validation failed: %s"), *OutError);
    return false;
  }

  UE_LOG(LogStageShowSimulator, Log, TEXT("Loaded robot pose track: %d samples from %s"), OutTrack.Samples.Num(), *NormalizedPath);
  return true;
}
