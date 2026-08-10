// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShowClockComponent.generated.h"

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
  void Seek(float InTimeSeconds);

  UFUNCTION(BlueprintCallable, Category="ShowClock")
  void AdvanceTime(float DeltaTimeSeconds);

  UFUNCTION(BlueprintPure, Category="ShowClock")
  float GetCurrentTimeSeconds() const;

  UFUNCTION(BlueprintPure, Category="ShowClock")
  bool IsPlaying() const;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ShowClock")
  bool bAutoPlay = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ShowClock", meta=(ClampMin="0.0"))
  float PlaybackLengthSeconds = 0.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ShowClock")
  float PlaybackRate = 1.0f;

private:
  float CurrentTimeSeconds = 0.0f;
  bool bPlaying = false;

  float ClampTime(float InTimeSeconds) const;
};
