// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ArtNet/ArtNetTypes.h"
#include "ArtNetReceiverComponent.generated.h"

class FSocket;

UCLASS(ClassGroup=(StageShow), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class STAGESHOWSIMULATOR_API UArtNetReceiverComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  UArtNetReceiverComponent();

  virtual void BeginPlay() override;
  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

  UFUNCTION(BlueprintCallable, Category="ArtNet")
  bool GetChannelValue(int32 Universe, int32 ChannelOneBased, int32& OutValue) const;

  UFUNCTION(BlueprintPure, Category="ArtNet")
  bool GetLatestFrame(int32 Universe, FArtNetDmxFrame& OutFrame) const;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ArtNet")
  FString BindAddress = TEXT("127.0.0.1");

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ArtNet", meta=(ClampMin="1", ClampMax="65535"))
  int32 Port = 6454;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ArtNet")
  int32 UniverseFilter = -1;

private:
  FSocket* ReceiveSocket = nullptr;
  TMap<int32, FArtNetDmxFrame> LatestFrames;
  TArray<uint8> ReceiveBuffer;

  bool InitializeSocket();
  void ShutdownSocket();

#if WITH_DEV_AUTOMATION_TESTS
public:
  void SetLatestFrameForTesting(const FArtNetDmxFrame& InFrame);
  bool ShouldAcceptUniverseForTesting(int32 InUniverse) const;
#endif
};
