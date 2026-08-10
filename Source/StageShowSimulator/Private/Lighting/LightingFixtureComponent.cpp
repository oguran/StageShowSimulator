// Copyright Epic Games, Inc. All Rights Reserved.

#include "Lighting/LightingFixtureComponent.h"

#include "ArtNet/ArtNetReceiverComponent.h"
#include "Components/LightComponent.h"
#include "GameFramework/Actor.h"

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
  }
}

void ULightingFixtureComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  if (Receiver == nullptr || TargetLight == nullptr)
  {
    return;
  }

  int32 ChannelValue = 0;
  if (!Receiver->GetChannelValue(Universe, IntensityChannel, ChannelValue))
  {
    return;
  }

  const float Alpha = FMath::Clamp(static_cast<float>(ChannelValue) / 255.0f, 0.0f, 1.0f);
  TargetLight->SetIntensity(Alpha * MaxIntensity);
}
