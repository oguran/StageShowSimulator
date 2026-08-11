// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShowClockComponent.generated.h"

UENUM(BlueprintType)
enum class EShowClockState : uint8
{
  Stopped UMETA(DisplayName="Stopped"),
  Playing UMETA(DisplayName="Playing"),
  Paused UMETA(DisplayName="Paused"),
  Seeking UMETA(DisplayName="Seeking"),
  Error UMETA(DisplayName="Error")
};

UCLASS(ClassGroup=(StageShow), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class STAGESHOWSIMULATOR_API UShowClockComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  UShowClockComponent();

 virtual void BeginPlay() override;
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

  UFUNCTION(BlueprintCallable, Category="ShowClock")
  void Play();

  UFUNCTION(BlueprintCallable, Category="ShowClock")
  void Pause();

  UFUNCTION(BlueprintCallable, Category="ShowClock")
  void Stop();

  UFUNCTION(BlueprintCallable, Category="ShowClock")
  void Seek(double InTimeSeconds);

  UFUNCTION(BlueprintCallable, Category="ShowClock")
  void AdvanceTime(double DeltaTimeSeconds);

  UFUNCTION(BlueprintPure, Category="ShowClock")
  double GetCurrentTimeSeconds() const;

  UFUNCTION(BlueprintPure, Category="ShowClock")
  bool IsPlaying() const;

  UFUNCTION(BlueprintPure, Category="ShowClock")
  EShowClockState GetState() const;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ShowClock")
  bool bAutoPlay = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ShowClock", meta=(ClampMin="0.0"))
  double PlaybackLengthSeconds = 0.0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ShowClock")
  double PlaybackRate = 1.0;

private:
  double CurrentTimeSeconds = 0.0;
  EShowClockState State = EShowClockState::Stopped;

  double ClampTime(double InTimeSeconds) const;
};
