// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LightingFixtureComponent.generated.h"

class UArtNetReceiverComponent;
class ULightComponent;

UCLASS(ClassGroup=(StageShow), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class STAGESHOWSIMULATOR_API ULightingFixtureComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  ULightingFixtureComponent();

  virtual void BeginPlay() override;
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lighting")
  int32 Universe = 0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lighting")
  int32 IntensityChannel = 1;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lighting")
  float MaxIntensity = 50000.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lighting")
  TObjectPtr<UArtNetReceiverComponent> Receiver = nullptr;

private:
  TObjectPtr<ULightComponent> TargetLight = nullptr;
  bool bLoggedMissingReceiver = false;
  bool bLoggedMissingTargetLight = false;
};
