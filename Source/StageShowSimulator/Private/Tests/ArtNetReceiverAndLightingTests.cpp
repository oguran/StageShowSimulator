// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "ArtNet/ArtNetReceiverComponent.h"
#include "Lighting/LightingFixtureComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/Actor.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FArtNetReceiverChannelIndexingTest, "StageShowSimulator.ArtNet.Receiver.ChannelIndexing", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FArtNetReceiverUniverseFilterTest, "StageShowSimulator.ArtNet.Receiver.UniverseFilter", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLightingFixtureReceiverAutoDiscoveryTest, "StageShowSimulator.Lighting.ReceiverAutoDiscovery", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FArtNetReceiverChannelIndexingTest::RunTest(const FString& Parameters)
{
  UArtNetReceiverComponent* Receiver = NewObject<UArtNetReceiverComponent>();
  TestNotNull(TEXT("Receiver should be created"), Receiver);
  if (Receiver == nullptr)
  {
    return false;
  }

  FArtNetDmxFrame Frame;
  Frame.Universe = 0;
  Frame.Channels = { 11, 22, 33 };
  Receiver->SetLatestFrameForTesting(Frame);

  int32 Value = 0;
  TestTrue(TEXT("Channel 1 should be readable"), Receiver->GetChannelValue(0, 1, Value));
  TestEqual(TEXT("Channel 1 should map to index 0"), Value, 11);

  TestTrue(TEXT("Channel 3 should be readable"), Receiver->GetChannelValue(0, 3, Value));
  TestEqual(TEXT("Channel 3 should map to index 2"), Value, 33);

  TestFalse(TEXT("Channel 0 should be rejected (one-based input)"), Receiver->GetChannelValue(0, 0, Value));
  return true;
}

bool FArtNetReceiverUniverseFilterTest::RunTest(const FString& Parameters)
{
  UArtNetReceiverComponent* Receiver = NewObject<UArtNetReceiverComponent>();
  TestNotNull(TEXT("Receiver should be created"), Receiver);
  if (Receiver == nullptr)
  {
    return false;
  }

  Receiver->UniverseFilter = -1;
  TestTrue(TEXT("UniverseFilter=-1 should accept all universes"), Receiver->ShouldAcceptUniverseForTesting(0));
  TestTrue(TEXT("UniverseFilter=-1 should accept all universes (non-zero)"), Receiver->ShouldAcceptUniverseForTesting(7));

  Receiver->UniverseFilter = 0;
  TestTrue(TEXT("UniverseFilter=0 should accept universe 0"), Receiver->ShouldAcceptUniverseForTesting(0));
  TestFalse(TEXT("UniverseFilter=0 should reject other universes"), Receiver->ShouldAcceptUniverseForTesting(1));

  Receiver->UniverseFilter = 2;
  TestTrue(TEXT("UniverseFilter=2 should accept universe 2"), Receiver->ShouldAcceptUniverseForTesting(2));
  TestFalse(TEXT("UniverseFilter=2 should reject universe 1"), Receiver->ShouldAcceptUniverseForTesting(1));

  return true;
}

bool FLightingFixtureReceiverAutoDiscoveryTest::RunTest(const FString& Parameters)
{
  AActor* Owner = NewObject<AActor>();
  TestNotNull(TEXT("Owner actor should be created"), Owner);
  if (Owner == nullptr)
  {
    return false;
  }

  UArtNetReceiverComponent* Receiver = NewObject<UArtNetReceiverComponent>(Owner);
  UPointLightComponent* PointLight = NewObject<UPointLightComponent>(Owner);
  ULightingFixtureComponent* Lighting = NewObject<ULightingFixtureComponent>(Owner);

  TestNotNull(TEXT("Receiver component should be created"), Receiver);
  TestNotNull(TEXT("PointLight component should be created"), PointLight);
  TestNotNull(TEXT("Lighting fixture component should be created"), Lighting);
  if (Receiver == nullptr || PointLight == nullptr || Lighting == nullptr)
  {
    return false;
  }

  Owner->AddOwnedComponent(Receiver);
  Owner->AddOwnedComponent(PointLight);
  Owner->AddOwnedComponent(Lighting);

  Lighting->Receiver = nullptr;
  Lighting->BeginPlay();

  TestEqual(TEXT("Lighting fixture should auto-discover receiver from owner"), Lighting->Receiver.Get(), Receiver);
  return true;
}

#endif
