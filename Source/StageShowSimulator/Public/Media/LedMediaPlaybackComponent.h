// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LedMediaPlaybackComponent.generated.h"

class UMediaPlayer;
class UMediaSource;

UCLASS(ClassGroup=(StageShow), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class STAGESHOWSIMULATOR_API ULedMediaPlaybackComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  ULedMediaPlaybackComponent();

  virtual void BeginPlay() override;
  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

  UFUNCTION(BlueprintCallable, Category="Media")
  bool OpenMedia();

  UFUNCTION(BlueprintCallable, Category="Media")
  bool PlayMedia();

  UFUNCTION(BlueprintCallable, Category="Media")
  bool PauseMedia();

  UFUNCTION(BlueprintCallable, Category="Media")
  bool StopMedia();

  UFUNCTION(BlueprintCallable, Category="Media")
  bool SeekMedia(double InTimeSeconds);

  UFUNCTION(BlueprintCallable, Category="Media")
  void SetMediaPlayer(UMediaPlayer* NewMediaPlayer);

  UFUNCTION(BlueprintPure, Category="Media")
  double GetCurrentMediaTimeSeconds() const;

  UFUNCTION(BlueprintPure, Category="Media")
  double GetDurationSeconds() const;

  UFUNCTION(BlueprintPure, Category="Media")
  bool IsMediaPlayerValid() const;

  UFUNCTION(BlueprintPure, Category="Media")
  bool IsMediaReady() const;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Media")
  TObjectPtr<UMediaPlayer> MediaPlayer = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Media")
  TObjectPtr<UMediaSource> MediaSource = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Media")
  FString MediaFilePath;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Media")
  bool bOpenOnBeginPlay = false;

  UPROPERTY(BlueprintReadOnly, Category="Media")
  bool bIsOpenPending = false;

  UPROPERTY(BlueprintReadOnly, Category="Media")
  bool bIsOpened = false;

private:
  void BindMediaPlayerDelegates();
  void UnbindMediaPlayerDelegates();

  UFUNCTION()
  void HandleMediaOpened(FString OpenedUrl);

  UFUNCTION()
  void HandleMediaOpenFailed(FString FailedUrl);
};
