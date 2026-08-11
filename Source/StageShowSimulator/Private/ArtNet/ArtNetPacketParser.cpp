// Copyright Epic Games, Inc. All Rights Reserved.

#include "ArtNet/ArtNetPacketParser.h"

namespace
{
  constexpr int32 ArtNetHeaderSize = 18;
  constexpr ANSICHAR ArtNetId[8] = { 'A', 'r', 't', '-', 'N', 'e', 't', 0x00 };
  constexpr uint16 OpOutput = 0x5000;
  constexpr uint16 MinSupportedProtocolVersion = 14;
  constexpr int32 MinDmxDataLength = 2;
  constexpr int32 MaxDmxDataLength = 512;
}

bool FArtNetPacketParser::TryParseDmx(const TArray<uint8>& PacketBytes, FArtNetDmxFrame& OutFrame, FString& OutError)
{
  OutError.Empty();

  if (PacketBytes.Num() < ArtNetHeaderSize)
  {
    OutError = TEXT("Packet too short for Art-Net DMX.");
    return false;
  }

  for (int32 Index = 0; Index < 8; ++Index)
  {
    if (PacketBytes[Index] != static_cast<uint8>(ArtNetId[Index]))
    {
      OutError = TEXT("Art-Net ID mismatch.");
      return false;
    }
  }

  const uint16 OpCode = static_cast<uint16>(PacketBytes[8]) | (static_cast<uint16>(PacketBytes[9]) << 8);
  if (OpCode != OpOutput)
  {
    OutError = TEXT("Not an ArtDMX packet.");
    return false;
  }

  const uint16 ProtocolVersion = (static_cast<uint16>(PacketBytes[10]) << 8) | static_cast<uint16>(PacketBytes[11]);
  if (ProtocolVersion < MinSupportedProtocolVersion)
  {
    OutError = TEXT("Unsupported Art-Net protocol version.");
    return false;
  }

  const int32 DataLength = (static_cast<int32>(PacketBytes[16]) << 8) | static_cast<int32>(PacketBytes[17]);
  if (DataLength < MinDmxDataLength
    || DataLength > MaxDmxDataLength
    || (DataLength % 2) != 0
    || PacketBytes.Num() < ArtNetHeaderSize + DataLength)
  {
    OutError = TEXT("Invalid ArtDMX data length.");
    return false;
  }

  OutFrame.Sequence = PacketBytes[12];
  OutFrame.Universe = static_cast<int32>(PacketBytes[14]) | (static_cast<int32>(PacketBytes[15]) << 8);
  OutFrame.Channels.SetNumUninitialized(DataLength);
  if (DataLength > 0)
  {
    FMemory::Memcpy(OutFrame.Channels.GetData(), PacketBytes.GetData() + ArtNetHeaderSize, DataLength);
  }

  return true;
}
