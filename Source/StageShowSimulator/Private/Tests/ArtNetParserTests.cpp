// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "ArtNet/ArtNetPacketParser.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FArtNetPacketParseTest, "StageShowSimulator.ArtNet.PacketParse", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FArtNetPacketParseTest::RunTest(const FString& Parameters)
{
  auto BuildPacket = [](uint16 ProtocolVersion, uint16 Universe, const TArray<uint8>& Payload)
  {
    TArray<uint8> Packet;
    Packet.Append(reinterpret_cast<const uint8*>("Art-Net\0"), 8);
    Packet.Add(0x00);
    Packet.Add(0x50);
    Packet.Add(static_cast<uint8>((ProtocolVersion >> 8) & 0xFF));
    Packet.Add(static_cast<uint8>(ProtocolVersion & 0xFF));
    Packet.Add(0x01);
    Packet.Add(0x00);
    Packet.Add(static_cast<uint8>(Universe & 0xFF));
    Packet.Add(static_cast<uint8>((Universe >> 8) & 0xFF));
    Packet.Add(static_cast<uint8>((Payload.Num() >> 8) & 0xFF));
    Packet.Add(static_cast<uint8>(Payload.Num() & 0xFF));
    Packet.Append(Payload);
    return Packet;
  };

  TArray<uint8> Packet = BuildPacket(14, 1, { 10, 20 });

  FArtNetDmxFrame Frame;
  FString Error;
  TestTrue(TEXT("Valid ArtDMX packet should parse"), FArtNetPacketParser::TryParseDmx(Packet, Frame, Error));
  TestEqual(TEXT("Universe"), Frame.Universe, 1);
  TestEqual(TEXT("Sequence"), Frame.Sequence, 1);
  TestEqual(TEXT("Channel count"), Frame.Channels.Num(), 2);
  TestEqual(TEXT("First channel"), static_cast<int32>(Frame.Channels[0]), 10);
  TestEqual(TEXT("Second channel"), static_cast<int32>(Frame.Channels[1]), 20);

  Packet[0] = 'X';
  TestFalse(TEXT("Invalid header should fail"), FArtNetPacketParser::TryParseDmx(Packet, Frame, Error));

  Packet = BuildPacket(13, 1, { 10, 20 });
  TestFalse(TEXT("Unsupported protocol version should fail"), FArtNetPacketParser::TryParseDmx(Packet, Frame, Error));

  Packet = BuildPacket(14, 1, {});
  TestFalse(TEXT("Zero-length payload should fail"), FArtNetPacketParser::TryParseDmx(Packet, Frame, Error));

  Packet = BuildPacket(14, 1, { 10, 20, 30 });
  TestFalse(TEXT("Odd-length payload should fail"), FArtNetPacketParser::TryParseDmx(Packet, Frame, Error));

  TArray<uint8> OversizedPayload;
  OversizedPayload.Init(0, 514);
  Packet = BuildPacket(14, 1, OversizedPayload);
  TestFalse(TEXT("Payload larger than 512 channels should fail"), FArtNetPacketParser::TryParseDmx(Packet, Frame, Error));

  return true;
}

#endif
