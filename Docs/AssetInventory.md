# Asset Inventory

## Test Map

- Asset: /Game/StageShow/Maps/L_StageShowTest
- Purpose: Art-Net、Robot Motion、ShowClockの手動テスト

## Robot Blueprint

- Asset: /Game/StageShow/Robots/BP_RobotDisplay
- Components:
  - StaticMeshComponent
  - RobotPosePlaybackComponent
  - ShowClockComponent

## Art-Net Test Light

- Asset: /Game/StageShow/Lighting/BP_ArtNetTestLight
- Components:
  - PointLightComponent
  - ArtNetReceiverComponent
  - LightingFixtureComponent