// Copyright Epic Games, Inc. All Rights Reserved.

#include "ArtNet/ArtNetReceiverComponent.h"

#include "ArtNet/ArtNetPacketParser.h"
#include "Common/UdpSocketBuilder.h"
#include "IPAddress.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "StageShowSimulatorLog.h"

namespace
{
  constexpr int32 MaxReceivePacketSize = 2048;
  constexpr int32 MaxPacketsToProcessPerTick = 32;
}

UArtNetReceiverComponent::UArtNetReceiverComponent()
{
  PrimaryComponentTick.bCanEverTick = true;
}

void UArtNetReceiverComponent::BeginPlay()
{
  Super::BeginPlay();
  InitializeSocket();
}

void UArtNetReceiverComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  ShutdownSocket();
  Super::EndPlay(EndPlayReason);
}

void UArtNetReceiverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  if (ReceiveSocket == nullptr)
  {
    return;
  }

  uint32 PendingDataSize = 0;
  int32 ProcessedPackets = 0;
  while (ProcessedPackets < MaxPacketsToProcessPerTick && ReceiveSocket->HasPendingData(PendingDataSize))
  {
    const int32 ClampedSize = FMath::Min(static_cast<int32>(PendingDataSize), MaxReceivePacketSize);
    ReceiveBuffer.SetNumUninitialized(ClampedSize, EAllowShrinking::No);

    int32 BytesRead = 0;
    if (!ReceiveSocket->Recv(ReceiveBuffer.GetData(), ReceiveBuffer.Num(), BytesRead) || BytesRead <= 0)
    {
      break;
    }

    ++ProcessedPackets;

    ReceiveBuffer.SetNum(BytesRead, EAllowShrinking::No);

    FArtNetDmxFrame Frame;
    FString ParseError;
    if (!FArtNetPacketParser::TryParseDmx(ReceiveBuffer, Frame, ParseError))
    {
      continue;
    }

    if (UniverseFilter >= 0 && Frame.Universe != UniverseFilter)
    {
      continue;
    }

    LatestFrames.Add(Frame.Universe, MoveTemp(Frame));
  }
}

bool UArtNetReceiverComponent::GetChannelValue(int32 Universe, int32 ChannelOneBased, int32& OutValue) const
{
  OutValue = 0;

  const FArtNetDmxFrame* Frame = LatestFrames.Find(Universe);
  if (Frame == nullptr || ChannelOneBased <= 0)
  {
    return false;
  }

  const int32 Index = ChannelOneBased - 1;
  if (!Frame->Channels.IsValidIndex(Index))
  {
    return false;
  }

  OutValue = Frame->Channels[Index];
  return true;
}

bool UArtNetReceiverComponent::GetLatestFrame(int32 Universe, FArtNetDmxFrame& OutFrame) const
{
  const FArtNetDmxFrame* Frame = LatestFrames.Find(Universe);
  if (Frame == nullptr)
  {
    return false;
  }

  OutFrame = *Frame;
  return true;
}

bool UArtNetReceiverComponent::InitializeSocket()
{
  ShutdownSocket();

  if (Port < 1 || Port > 65535)
  {
    UE_LOG(LogStageShowSimulator, Error, TEXT("Invalid Art-Net UDP port: %d (expected 1..65535)"), Port);
    return false;
  }

 FIPv4Address ParsedAddress;
  if (!FIPv4Address::Parse(BindAddress, ParsedAddress))
  {
    UE_LOG(LogStageShowSimulator, Error, TEXT("Invalid Art-Net bind address: %s"), *BindAddress);
    return false;
  }

  const FIPv4Endpoint Endpoint(ParsedAddress, Port);

  ReceiveSocket = FUdpSocketBuilder(TEXT("StageShowSimulator_ArtNetReceiver"))
    .AsNonBlocking()
    .AsReusable()
    .BoundToEndpoint(Endpoint)
    .WithReceiveBufferSize(2 * 1024 * 1024);

  if (ReceiveSocket == nullptr)
  {
    UE_LOG(LogStageShowSimulator, Error, TEXT("Failed to create Art-Net UDP receive socket."));
    return false;
  }

  UE_LOG(LogStageShowSimulator, Log, TEXT("Art-Net receiver bound to %s:%d"), *BindAddress, Port);
  return true;
}

void UArtNetReceiverComponent::ShutdownSocket()
{
  ReceiveBuffer.Reset();

  if (ReceiveSocket != nullptr)
  {
    ReceiveSocket->Close();
    ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ReceiveSocket);
    ReceiveSocket = nullptr;
  }
}

#if WITH_DEV_AUTOMATION_TESTS
void UArtNetReceiverComponent::SetLatestFrameForTesting(const FArtNetDmxFrame& InFrame)
{
  LatestFrames.Add(InFrame.Universe, InFrame);
}

bool UArtNetReceiverComponent::ShouldAcceptUniverseForTesting(int32 InUniverse) const
{
  return UniverseFilter < 0 || InUniverse == UniverseFilter;
}
#endif
