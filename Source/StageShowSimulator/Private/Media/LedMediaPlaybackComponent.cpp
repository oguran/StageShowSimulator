// Copyright Epic Games, Inc. All Rights Reserved.

#include "Media/LedMediaPlaybackComponent.h"

#include "Media/LedMediaPlaybackUtilities.h"
#include "MediaPlayer.h"
#include "MediaSource.h"
#include "StageShowSimulatorLog.h"

ULedMediaPlaybackComponent::ULedMediaPlaybackComponent()
{
  PrimaryComponentTick.bCanEverTick = false;
}

void ULedMediaPlaybackComponent::BeginPlay()
{
  Super::BeginPlay();

  BindMediaPlayerDelegates();

  if (bOpenOnBeginPlay)
  {
    OpenMedia();
  }
}

void ULedMediaPlaybackComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  UnbindMediaPlayerDelegates();
  Super::EndPlay(EndPlayReason);
}

bool ULedMediaPlaybackComponent::OpenMedia()
{
  if (!IsMediaPlayerValid())
  {
    UE_LOG(LogStageShowSimulator, Warning, TEXT("LedMediaPlaybackComponent: MediaPlayer is not set."));
    return false;
  }

  if (bIsOpenPending)
  {
    UE_LOG(LogStageShowSimulator, Warning, TEXT("LedMediaPlaybackComponent: OpenMedia rejected because open is already pending."));
    return false;
  }

  if (MediaSource == nullptr && MediaFilePath.IsEmpty())
  {
    UE_LOG(LogStageShowSimulator, Warning, TEXT("LedMediaPlaybackComponent: OpenMedia rejected because MediaSource and MediaFilePath are both unset."));
    return false;
  }

  bIsOpenPending = true;
  bIsOpened = false;

  bool bOpenRequested = false;
  if (MediaSource != nullptr)
  {
    bOpenRequested = MediaPlayer->OpenSource(MediaSource);
  }
  else
  {
    bOpenRequested = MediaPlayer->OpenUrl(MediaFilePath);
  }

  if (!bOpenRequested)
  {
    bIsOpenPending = false;
    UE_LOG(LogStageShowSimulator, Warning, TEXT("LedMediaPlaybackComponent: Open request was rejected immediately by MediaPlayer."));
    return false;
  }

  return true;
}

bool ULedMediaPlaybackComponent::PlayMedia()
{
  if (!IsMediaPlayerValid())
  {
    UE_LOG(LogStageShowSimulator, Warning, TEXT("LedMediaPlaybackComponent: PlayMedia rejected because MediaPlayer is not set."));
    return false;
  }

  if (!IsMediaReady())
  {
    UE_LOG(LogStageShowSimulator, Warning, TEXT("LedMediaPlaybackComponent: PlayMedia rejected because media is not ready."));
    return false;
  }

  return MediaPlayer->Play();
}

bool ULedMediaPlaybackComponent::PauseMedia()
{
  if (!IsMediaPlayerValid())
  {
    UE_LOG(LogStageShowSimulator, Warning, TEXT("LedMediaPlaybackComponent: PauseMedia rejected because MediaPlayer is not set."));
    return false;
  }

  if (!IsMediaReady())
  {
    UE_LOG(LogStageShowSimulator, Warning, TEXT("LedMediaPlaybackComponent: PauseMedia rejected because media is not ready."));
    return false;
  }

  return MediaPlayer->Pause();
}

bool ULedMediaPlaybackComponent::StopMedia()
{
  if (!IsMediaPlayerValid())
  {
    UE_LOG(LogStageShowSimulator, Warning, TEXT("LedMediaPlaybackComponent: StopMedia rejected because MediaPlayer is not set."));
    return false;
  }

  if (!IsMediaReady())
  {
    UE_LOG(LogStageShowSimulator, Warning, TEXT("LedMediaPlaybackComponent: StopMedia rejected because media is not ready."));
    return false;
  }

  const bool bPauseResult = MediaPlayer->Pause();
  const bool bRewindResult = MediaPlayer->Rewind();
  return bPauseResult && bRewindResult;
}

bool ULedMediaPlaybackComponent::SeekMedia(double InTimeSeconds)
{
  if (!IsMediaPlayerValid())
  {
    UE_LOG(LogStageShowSimulator, Warning, TEXT("LedMediaPlaybackComponent: SeekMedia rejected because MediaPlayer is not set."));
    return false;
  }

  if (!IsMediaReady())
  {
    UE_LOG(LogStageShowSimulator, Warning, TEXT("LedMediaPlaybackComponent: SeekMedia rejected because media is not ready."));
    return false;
  }

  const double DurationSeconds = GetDurationSeconds();
  const double ClampedTimeSeconds = StageShowSimulatorMediaPlayback::ClampSeekTimeSeconds(InTimeSeconds, DurationSeconds);
  return MediaPlayer->Seek(FTimespan::FromSeconds(ClampedTimeSeconds));
}

void ULedMediaPlaybackComponent::SetMediaPlayer(UMediaPlayer* NewMediaPlayer)
{
  if (MediaPlayer == NewMediaPlayer)
  {
    return;
  }

  UnbindMediaPlayerDelegates();
  MediaPlayer = NewMediaPlayer;
  bIsOpenPending = false;
  bIsOpened = false;
  BindMediaPlayerDelegates();
}

double ULedMediaPlaybackComponent::GetCurrentMediaTimeSeconds() const
{
  if (!IsMediaReady())
  {
    return 0.0;
  }

  return MediaPlayer->GetTime().GetTotalSeconds();
}

double ULedMediaPlaybackComponent::GetDurationSeconds() const
{
  if (!IsMediaReady())
  {
    return 0.0;
  }

  return MediaPlayer->GetDuration().GetTotalSeconds();
}

bool ULedMediaPlaybackComponent::IsMediaPlayerValid() const
{
  return MediaPlayer != nullptr;
}

bool ULedMediaPlaybackComponent::IsMediaReady() const
{
  return MediaPlayer != nullptr && MediaPlayer->IsReady();
}

void ULedMediaPlaybackComponent::BindMediaPlayerDelegates()
{
  if (MediaPlayer == nullptr)
  {
    return;
  }

  MediaPlayer->OnMediaOpened.RemoveDynamic(this, &ULedMediaPlaybackComponent::HandleMediaOpened);
  MediaPlayer->OnMediaOpenFailed.RemoveDynamic(this, &ULedMediaPlaybackComponent::HandleMediaOpenFailed);

  MediaPlayer->OnMediaOpened.AddDynamic(this, &ULedMediaPlaybackComponent::HandleMediaOpened);
  MediaPlayer->OnMediaOpenFailed.AddDynamic(this, &ULedMediaPlaybackComponent::HandleMediaOpenFailed);
}

void ULedMediaPlaybackComponent::UnbindMediaPlayerDelegates()
{
  if (MediaPlayer == nullptr)
  {
    return;
  }

  MediaPlayer->OnMediaOpened.RemoveDynamic(this, &ULedMediaPlaybackComponent::HandleMediaOpened);
  MediaPlayer->OnMediaOpenFailed.RemoveDynamic(this, &ULedMediaPlaybackComponent::HandleMediaOpenFailed);
}

void ULedMediaPlaybackComponent::HandleMediaOpened(FString OpenedUrl)
{
  bIsOpenPending = false;
  bIsOpened = true;
  UE_LOG(LogStageShowSimulator, Log, TEXT("LedMediaPlaybackComponent: Media opened successfully."));
}

void ULedMediaPlaybackComponent::HandleMediaOpenFailed(FString FailedUrl)
{
  bIsOpenPending = false;
  bIsOpened = false;
  UE_LOG(LogStageShowSimulator, Error, TEXT("LedMediaPlaybackComponent: Media open failed."));
}
