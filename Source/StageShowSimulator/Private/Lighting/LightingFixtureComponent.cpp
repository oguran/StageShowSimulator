// Copyright Epic Games, Inc. All Rights Reserved.

#include "Lighting/LightingFixtureComponent.h"

#include "ArtNet/ArtNetReceiverComponent.h"
#include "Components/LightComponent.h"
#include "GameFramework/Actor.h"
#include "StageShowSimulatorLog.h"

ULightingFixtureComponent::ULightingFixtureComponent()
{
  PrimaryComponentTick.bCanEverTick = true;
}

void ULightingFixtureComponent::BeginPlay()
{
  Super::BeginPlay();

  if (AActor* Owner = GetOwner())
  {
    TargetLight = Owner->FindComponentByClass<ULightComponent>();

    if (Receiver == nullptr)
    {
      Receiver = Owner->FindComponentByClass<UArtNetReceiverComponent>();
    }
  }
}

void ULightingFixtureComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  if (Receiver == nullptr)
  {
    if (!bLoggedMissingReceiver)
    {
      UE_LOG(LogStageShowSimulator, Warning, TEXT("Receiver is null."));
      bLoggedMissingReceiver = true;
    }
    return;
  }
  bLoggedMissingReceiver = false;

  if (TargetLight == nullptr)
  {
    if (!bLoggedMissingTargetLight)
    {
      UE_LOG(LogStageShowSimulator, Warning, TEXT("TargetLight is null."));
      bLoggedMissingTargetLight = true;
    }
    return;
  }
  bLoggedMissingTargetLight = false;

  int32 ChannelValue = 0;
  if (!Receiver->GetChannelValue(Universe, IntensityChannel, ChannelValue))
  {
    return;
  }

  const float Alpha = FMath::Clamp(static_cast<float>(ChannelValue) / 255.0f, 0.0f, 1.0f);
  TargetLight->SetIntensity(Alpha * MaxIntensity);
}
