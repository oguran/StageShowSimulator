# Bootstrap Manual Test

## Environment

- Unreal Engine: 5.7
- QLC+: 5.2.2
- Art-Net destination: 127.0.0.1
- UDP port: 6454
- Art-Net universe: 0
- QLC+ universe: 1

## Test results

### Robot JSON

- JSON load: Pass
- Position interpolation: Pass
- Yaw interpolation: Pass
- Play: Pass
- Pause: Pass
- Stop: Pass
- Seek: Pass

### Art-Net

- UDP bind: Pass
- Channel 1 reception: Pass
- Light intensity update: Pass
- Second PIE launch: Pass
- Socket cleanup: Pass

## Issues

- Nothing

## Logs
LogDebuggerCommands: Repeating last play command: 選択ビューポート
LogCameraSystemEditor: No camera objects needed building (inspected 0 objects)
LogPlayLevel: PlayLevel: No blueprints needed recompiling
LogPlayLevel: Creating play world package: /Game/StageShow/Maps/UEDPIE_0_L_StageShowTest
LogPlayLevel: PIE: StaticDuplicateObject took: (0.002258s)
LogPlayLevel: PIE: Created PIE world by copying editor world from /Game/StageShow/Maps/L_StageShowTest.L_StageShowTest to /Game/StageShow/Maps/UEDPIE_0_L_StageShowTest.L_StageShowTest (0.002290s)
LogUObjectHash: Compacting FUObjectHashTables data took   1.01ms
LogChaosDD: Creating Chaos Debug Draw Scene for world L_StageShowTest
LogPlayLevel: PIE: World Init took: (0.001678s)
LogAudio: Display: Creating Audio Device:                 Id: 45, Scope: Unique, Realtime: True
LogAudioMixer: Display: Audio Mixer Platform Settings:
LogAudioMixer: Display:     Sample Rate:                          48000
LogAudioMixer: Display:     Callback Buffer Frame Size Requested: 1024
LogAudioMixer: Display:     Callback Buffer Frame Size To Use:    1024
LogAudioMixer: Display:     Number of buffers to queue:           1
LogAudioMixer: Display:     Max Channels (voices):                32
LogAudioMixer: Display:     Number of Async Source Workers:       4
LogAudio: Display: AudioDevice MaxSources: 32
LogAudio: Display: Audio Spatialization Plugin: None (built-in).
LogAudio: Display: Audio Reverb Plugin: None (built-in).
LogAudio: Display: Audio Occlusion Plugin: None (built-in).
LogAudioMixer: Display: Initializing audio mixer using platform API: 'XAudio2'
LogAudioEnumeration: Display: FWindowsMMDeviceCache: Default Render Role='Console', Device='リモート オーディオ'
LogAudioEnumeration: Display: FWindowsMMDeviceCache: Default Capture Role='Console', Device='リモート オーディオ'
LogAudioEnumeration: Display: FWindowsMMDeviceCache: Default Render Role='Multimedia', Device='リモート オーディオ'
LogAudioEnumeration: Display: FWindowsMMDeviceCache: Default Capture Role='Multimedia', Device='リモート オーディオ'
LogAudioEnumeration: Display: FWindowsMMDeviceCache: Default Render Role='Communications', Device='リモート オーディオ'
LogAudioEnumeration: Display: FWindowsMMDeviceCache: Default Capture Role='Communications', Device='リモート オーディオ'
LogAudioMixer: Display: Using Audio Hardware Device リモート オーディオ
LogAudioMixer: Display: Initializing Sound Submixes...
LogAudioMixer: Display: Creating Master Submix 'MasterSubmixDefault'
LogAudioMixer: Display: Creating Master Submix 'MasterReverbSubmixDefault'
LogAudioMixer: FMixerPlatformXAudio2::StartAudioStream() called. InstanceID=45
LogAudioMixer: Display: Output buffers initialized: Frames=1024, Channels=2, Samples=2048, InstanceID=45
LogAudioMixer: Display: Starting AudioMixerPlatformInterface::RunInternal(), InstanceID=45
LogInit: FAudioDevice initialized with ID 45.
LogAudioMixer: Display: FMixerPlatformXAudio2::SubmitBuffer() called for the first time. InstanceID=45
LogAudio: Display: Audio Device (ID: 45) registered with world 'L_StageShowTest'.
LogAudioMixer: Initializing Audio Bus Subsystem for audio device with ID 45
LogLoad: Game class is 'GameModeBase'
LogWorld: Bringing World /Game/StageShow/Maps/UEDPIE_0_L_StageShowTest.L_StageShowTest up for play (max tick rate 0) at 2026.08.10-19.24.56
LogWorld: Bringing up level for play took: 0.000819
LogOnline: OSS: Created online subsystem instance for: :Context_60
LogStageShowSimulator: Loaded robot pose track: 3 samples from D:/Project/StageShowSimulator/TestData/RobotPose/robot_pose_test_normal_8sec.json
LogStageShowSimulator: Art-Net receiver bound to 127.0.0.1:6454
PIE: サーバーにログインしました
PIE: PIE合計開始時間 0.103 秒。
LogRenderer: Recreating Persistent SBTs due to initializer changes: 
        NumShaderSlotsPerGeometrySegment changed: current: 1 - new: 2
        NumGeometrySegments changed: current: 0 - new: 512
LogSlate: Updating window title bar state: overlay mode, drag disabled, window buttons hidden, title bar hidden
LogWorld: BeginTearingDown for /Game/StageShow/Maps/UEDPIE_0_L_StageShowTest
LogWorld: UWorld::CleanupWorld for L_StageShowTest, bSessionEnded=true, bCleanupResources=true
LogSlate: InvalidateAllWidgets triggered.  All widgets were invalidated
LogPlayLevel: Display: Shutting down PIE online subsystems
LogSlate: InvalidateAllWidgets triggered.  All widgets were invalidated
LogAudio: Display: Audio Device unregistered from world 'None'.
LogAudioMixer: Deinitializing Audio Bus Subsystem for audio device with ID 45
LogAudioMixer: Display: FMixerPlatformXAudio2::StopAudioStream() called. InstanceID=45, StreamState=4
LogAudioMixer: Display: FMixerPlatformXAudio2::StopAudioStream() called. InstanceID=45, StreamState=2
LogSlate: Updating window title bar state: overlay mode, drag disabled, window buttons hidden, title bar hidden
LogUObjectHash: Compacting FUObjectHashTables data took   1.60ms
LogPlayLevel: Display: Destroying online subsystem :Context_60